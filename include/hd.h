#ifndef _OWINDYS_HD_H_
#define _OWINDYS_HD_H_

#include "type.h"

#define REG_DATA        0x1f0
#define REG_FEATURES    0x1f1
#define REG_NSECTOR     0x1f2
#define REG_LBA_LOW     0x1f3
#define REG_LBA_MID     0x1f4
#define REG_LBA_HIGH    0x1f5
#define REG_DEVICE      0x1f6
#define REG_STATUS      0x1f7
#define REG_ERROR       REG_FEATURS
#define REG_CMD         REG_STATUS
#define REG_DEV_CTRL    0x3f6
#define REG_DRV_ADDR    0x3f7
#define REG_ALT_STATUS  REG_DEV_CTRL

#define STATUS_BSY      0x80
#define STATUS_DRDY     0x40
#define STATUS_DFSE     0x20
#define STATUS_DSC      0x10
#define STATUS_DRQ      0x08
#define STATUS_CORR     0x04
#define STATUS_IDX      0x02
#define STATUS_ERR      0x01

#define MAX_IO_BYTES    0x100
#define HD_TIMEOUT      10000
#define PARTITION_TABLE_OFFSET  0x1be
#define ATA_IDENTIFY    0xec
#define ATA_READ        0x20
#define ATA_WRITE       0x30
#define MAX_DRIVES      2
#define NR_PART_PER_DRIVE   4
#define NR_SUB_PER_PART     16
#define NR_SUB_PER_DRIVE    (NR_SUB_PER_PART*NR_PART_PER_DRIVE)
#define NR_PRIM_PER_DRIVE   (NR_PART_PER_DRIVE+1)
#define MAX_PRIM            (MAX_DRIVES*NR_PRIM_PER_DRIVE-1)
#define MAX_SUBPARTITIONS   (NR_SUB_PER_DRIVE*MAX_DRIVES)

#define MAKE_DEVICE_REG(lba,drv,lba_highest)    (((lba)<<6)| \
                                            ((drv)<<4) \
                                            (lba_highest&0xf)|0xa)

struct part_ent {
    u8_t boot_ind;
    u8_t start_head;
    u8_t start_sector;
    u8_t start_cyl;
    u8_t sys_id;
    u8_t end_head;
    u8_t end_sector;
    u8_t end_cyl;
    u32_t start_sect;
    u32_t nr_sects;
} PARTITION_ENTRY;

struct hd_cmd {
    u8_t features;
    u8_t count;
    u8_t lba_low;
    u8_t lba_mid;
    u8_t lba_high;
    u8_t device;
    u8_t command;
};

struct part_info {
    u32_t base;
    u32_t size;
}

struct hd_info {
    int open_cnt;
    struct part_info    primary[NR_PRIM_PER_DRIVE];
    struct part_info    logical[NR_SUB_PER_DRIVE];
};

#endif

