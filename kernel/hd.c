#include "hd.h"
#include "util.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "tty.h"

PRIVATE void init_hd();
PRIVATE void hd_open(int device);
PRIVATE void hd_close(int device);
PRIVATE void hd_rdwt(MESSAGE *p);
PRIVATE void hd_ioctl(MESSAGE *p);
PRIVATE void hd_cmd_out(struct hd_cmd *cmd);
PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent *entry);
PRIVATE void partition(int device, int style);
PRIVATE void print_hdinfo(struct hd_info *hdi);
PRIVATE int waitfor(int mask, int val, int timeout);
PRIVATE void interrupt_wait();
PRIVATE void print_identify_info(u16_t *hdinfo);

PRIVATE u8_t hd_status;
PRIVATE u8_t hdbuf[SECTOR_SIZE << 1];
PRIVATE struct hd_info hd_info[1];

#define DRV_OF_DEV(dev) (dev<=MAX_PRIM? \
                    dev/NR_PRIM_PER_DRIVE: \
                    (dev-MINOR_hd1a)/NR_SUB_PER_DRIVE)

PUBLIC void task_hd()
{
    MESSAGE msg;
    init_hd();

    while (true) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case DEV_OPEN:
            hd_identify(0);
            //hd_open(msg.DEVICE);
            break;
        case DEV_CLOSE:
            hd_close(msg.DEVICE);
            break;
        case DEV_READ:
        case DEV_WRITE:
            hd_rdwt(&msg);
            break;
        case DEV_IOCTL:
            hd_ioctl(&msg);
            break;
        default:
            kprintf("[KERNEL ERROR]HD driveer::unknown msg");
            break;
        }

        send_recv(SEND, src, &msg);
    }
}

PRIVATE void init_hd()
{
    int i;
    
    u8_t *pNrDrives = (u8_t*)(0x475);
    kprintf("NrDrives:%d.\n", *pNrDrives);
    
    put_irq_handler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);

    for (i = 0; i < (sizeof (hd_info) / sizeof (hd_info[0])); ++i)
        memset(&hd_info[i], 0, sizeof (hd_info[0]));
    hd_info[0].open_cnt = 0;
}

PRIVATE void hd_open(int device)
{
    int drive = DRV_OF_DEV(device);

    hd_identify(drive);

    if (hd_info[drive].open_cnt++ == 0) {
        partition(drive * (NR_PART_PER_DRIVE + 1), P_PRIMARY);
        print_hdinfo(&hd_info[drive]);
    }
}

PRIVATE void hd_close(int device)
{
    int drive = DRV_OF_DEV(device);

    hd_info[drive].open_cnt--;
}

PRIVAATE void hd_rdwt(MESSAGE *p)
{
    int drive = DRV_OF_DEV(p->DEVICE);
    
    u64_t pos = p->POSITION;
    
    u32_t sect_nr = (u32_t)(pos >> SECTOR_SIZE_SHIFT);

    int logidx = (p->DEVICE - MINOR_HD1a) % NR_SUB_PER_DRIVE;
    sect_nr += p->DEVICE < MAX_PRIM ?
        hd_info[drive].primary[p->DEVICE].base :
        hd_info[drive].logical[logidx].base;

    struct hd_cmd cmd;
    cmd.features = 0;
    cmd.count = (p->CNT + SECTOR_SIZE - 1) / SECTOR_SIZE;
    cmd.lba_low = sect_nr & 0xff;
    cmd.lba_mid = (sect_nr >> 8) & 0xff;
    cmd.lba_high = (sect_nr >> 16) & 0xff;
    cmd.device = MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0xf);
    cmd.command = (p->type == DEV_READ) ? ATA_READ : ATA_WRITE;
    hd_cmd_out(&cmd);

    int bytes_left = p->CNT;
    void *la = (void*)va2la(p->PROC_NR, p->BUF);

    while (bytes_left) {
        int bytes = min(SECTOR_SIZE, bytes_left);
        if (p->type == DEV_READ) {
            interrupt_wait();
            port_read(REG_DATA, hdbuf, SECTOR_SIZE);
            phys_copy(la, (void*)va2la(TASK_HD, hdbuf), bytes);
        } else {
            if (!waitfor(STATUS_DRQ, STATUS_DRQ, HD_TIMEOUT)) {
                kprintf("[KERNEL ERROR]hd writing error.\n");
                break;
            }
            port_write(REG_DATA, la, bytes);
            interrupt_wait();
        }
        bytes_left -= SECTOR_SIZE;
        la += SECTOR_SIZE;
    }
}

PRIVATE void hd_ioctl(MESSAGE *p)
{
    int device = p->DEVICE;
    int drive = DRV_OF_DEV(device);

    struct hd_info *hdi = &hdinfo[drive];

    if (p->REQUEST == DIOCTL_GET_GEO) {
        void *dst = va2la(p->PROC_NR, p->BUF);
        void *src = va2la(TASK_HD,
                device < MAX_PRIM ?
                &hdi->primary[device] :
                &hdi->logical[(device - MINOR_hd1a) % NR_SUB_PER_DRIVE]);
        phys_copy(dst, src, sizeof (struct part_info));
    } else {
        kprintf("[KERNEL ERROR]error ioctl cmd\n");
    }
}

PRIVATE void get_part_table(int drive, int sectnr, struct part_ent *entry)
{
    struct hd_cmd cmd;
    cmd.features = 0;
    cmd.count = 1;
    cmd.lba_low = sect_nr & 0xff;
    cmd.lba_mid = (sect_nr >> 8) & 0xff;
    cmd.lba_high = (sect_nr >> 16) & 0xff;
    cmd.device = MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0xf);
    cmd.command = ATA_READ;
    hd_cmd_out(&cmd);
    interrupt_wait();

    port_read(REG_DATA, hdbuf, SECTOR_SIZE);
    memcpy(entry, hdbuf + PARTITION_TABLE_OFFSET, sizeof (struct part_ent) * NR_PART_PER_DRIVE);
}

PRIVATE void partition(int device, int style)
{
    int i;
    int drive = DRV_OF_DEV(device);
    struct hd_info *hdi = &hd_info[drive];

    struct part_ent part_tbl[NR_SUB_PER_DRIVE];

    if (style == P_PRIMARY) {
        get_part_table(drive, drive, part_tbl);

        int nr_prim_parts = 0;
        for (i = 0; i < NR_PART_PER_DRIVE; ++i) {
            if (part_tbl[i].sys_id == NO_PART)
                continue;

            nr_prim_parts++;
            int dev_nr = i + 1;
            hdi->primary[dev_nr].base = part_tbl[i].start_sect;
            hdi->primary[dev_nr].size = part_tbl[i].nr_sects;

            if (part_tbl[i].sys_id == EXT_PART)
                partition(device + dev_nr, P_EXTENDED);
        }
    } else if (style == P_EXTENDED) {
        int j = device % NR_PRIM_PER_DRIVE;
        int ext_start_sect = hdi->primary[j].base;
        int s = ext_start_sect;
        int nr_1st_sub = (j - 1) * NR_SUB_PER_PART;

        for (i = 0; i < NR_SUB_PER_PART; ++i) {
            int dev_nr = nr_1st_sub + i;

            get_part_table(drive, s, part_tbl);

            hdi->logical[dev_nr].base = s + part_tbl[0].start_sect;
            hdi->logical[dev_nr].size = part_tbl[0].nr_sects;

            s = ext_start_sect + part_tbl[1].start_sect;

            if (part_tbl[1].sys_id == NO_PART)
                break;
        }
    } else {
        kprintf("[KERNEL ERROR]error partition style\n");
    }
}

PRIVATE void print_hdinfo(struct hd_info *hdi)
{
    int i;
    for (i = 0; i < NR_PART_PER_DRIVE + 1; ++i) {
        kprintf("%sPART_d: base %d(%x), size %d(%x) (in sector)\n",
                i == 0 ? " " : "     ",
                i,
                hdi->primary[i].base,
                hdi->primary[i].base,
                hdi->primary[i].size,
                hdi->primary[i].size);
    }
    for (i = 0; i < NR_SUB_PER_DRIVE; ++i) {
        if (hdi->logical[i].size == 0)
            continue;
        kprintf("         "
                "%d: base %d(%x), size %d(%x) (in sector)\n",
                i,
                hdi->logical[i].base,
                hdi->logical[i].base,
                hdi->logical[i].size,
                hdi->logical[i].size);
    }
}

PRIVATE void hd_identify(int drive)
{
    struct hd_cmd cmd;
    cmd.device = MAKE_DEVICE_REG(0, drive, 0);
    cmd.command = ATA_IDENTIFY;
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG, DATA, hdbuf, SECTOR_SIZE);

    print_identify_info((u16_t*)hdbuf);

    u16_t *hdinfo = (u16_t*)hdbuf;
    
    hd_info[drive].primary[0].base = 0;
    hd_info[drive].primary[0].size = ((int)hdinfo[61] << 16) + hdinfo[60];
}

PRIVATE void print_identify_info(u16_t *hdinfo)
{
    int i, k;
    char s[64];

    struct iden_info_ascii {
        int idx, len;
        char *desc;
    } iinfo[] = {{10, 20, "HD SN"},
                    {27, 40, "HD Model"}};

    for (k = 0; k < sizeof (iinfo) / sizeof (iinfo[0]); ++k) {
        char *p = (char *)&hdinfo[iinfo[k].idx];
        for (i = 0; i < iinfo[k].len / 2; ++i) {
            s[i << 1 | 1] = *p++;
            s[i << 1] = *p++;
        }
        s[i << 1] = 0';
        kprintf("%s: %s\n", iinfo[k].desc, s);
    }

    int capabilities = hdinfo[49];
    kprintf("LBA supported: %s\n",
            (capabilities & 0x0200) ? "Yes" : "No");

    int cmd_set_supported = hdinfo[83];
    kprintf("LBA48 supported: %s\n",
            (cmd_set_supported & 0x0400) ? "Yes" : "No");

    int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
    kprintf("HD size: %dMB\n", sectors * 512 / 1000000);
}

PRIVATE void hd_cmd_out(struct hd_cmd *cmd)
{
    if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT)) {
        kprintf("[KERNEL ERROR]hd error.\n");
        return;
    }

    outb(REG_DEV_CTRL, 0);
    outb(REG_FEATURES, cmd->features);
    outb(REG_NSECTOR, cmd->count);
    outb(REG_LBA_LOW, cmd->lba_low);
    outb(REG_LBA_MID, cmd->lba_mid);
    outb(REG_LBA_HIGH, cmd->lba_high);
    outb(REG_DEVICE, cmd->device);
    outb(REG_CMD, cmd->command);
}

PRIVATE void interrupt_wait()
{
    MESSAGE msg;
    send_recv(RECEIVE, INTERRUPT, &msg);
}

PRIVATE int waitfor(int mask, int val, int timeout)
{
    int t = get_ticks();

    while (((get_ticks() - t) * 1000 / HZ) < timeout) {
        if ((inb(REG_STATUS) & mask) == val) {
            return 1;
        }
    }

    return 0;
}

PUBLIC void hd_handler(int irq)
{
    hd_status = inb(REG_STATUS);
    inform_int(TASK_HD);
}


