//
//-----------------------------------------------------------------
//                            main.c
//-----------------------------------------------------------------
//

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"
#include "global.h"
#include "util.h"

PUBLIC void set_base_size(DESCRIPTOR *d, int *base, int *size)
{
	int limit = (d->limit_high_attr2  & 0xF) << 16 | d->limit_low;

	*base = d->base_high << 24 | d->base_mid << 16 | d->base_low;

	*size = (limit + 1) * ((d->limit_high_attr2 & (DA_LIMIT_4K >> 8)) ? 4096 : 1);
} // DA_LIMIT_4K = 0x8000

/* 用kprintf输出信息是不合适的！
 * 或许还应该获得内存的实际大小！
 * 话说NR_NATIVE_PROCS还需要修改！
 */
PUBLIC int alloc_mem(int pid, int memsize)
{
	if (memsize > PROC_DEFAULT_MEM)
		kprintf("The memory applied is too large!");

	int base = PROCS_BASE + (pid - (NR_TASKS + NR_NATIVE_PROCS)) * PROC_DEFAULT_MEM;

	//if (base + memsize >= memory_size)
	//	kprintf("Memory is running out!");

	return base;
}

PUBLIC int do_fork()
{
	int i;
	PROCESS *p = proc_table;

	for (i = 0; i < NR_TASKS + NR_PROCS; i++, p++)
		if (p->p_flags == EMPTY)
			break;

	if (i == NR_TASKS + NR_PROCS) // proc_table已满，fork不成功
		return -1;

	int child_pid = i;
	int parent_pid = mm_msg.source;

	u16_t child_ldt_sel = p->ldt_sel;

	*p = proc_table[parent_pid]; // 复制父进程的进程表给子进程
	p->p_parent = parent_pid; 
	p->ldt_sel = child_ldt_sel; // 子进程的 LDT 选择子

	/* 下面通过读取父进程的ldt，得到父进程内存占用情况 */

	DESCRIPTOR *parent_ldt;

	int parent_base, parent_size;
	parent_ldt = &proc_table[parent_pid].ldts[INDEX_LDT_C];
	set_base_size(parent_ldt, &parent_base, &parent_size);

	/* 给子进程分配内存, 代码、数据、堆栈共用 */
	int child_base = alloc_mem(child_pid, parent_size);

	/* 拷贝父进程内存空间到子进程内存空间 */
	memcpy((void *)child_base, (void *)parent_base, parent_size);

	/* 子进程 LDT 
	init_descriptor(&p->ldts[INDEX_LDT_C],
			child_base,
			(PROC_DEFAULT_MEM - 1) >> LIMIT_4K_SHIFT,
			DA_LIMIT_4K | DA_32 | DA_C | PRIVILEGE_USER << 5);

	init_descriptor(&p->ldts[INDEX_LDT_RW],
			child_base,
			(PROC_DEFAULT_MEM - 1) >> LIMIT_4K_SHIFT,
			DA_LIMIT_4K | DA_32 | DA_DRW | PRIVILEGE_USER << 5);

	*/
	/* >>>>> 之后还应该处理有关文件的问题 <<<<< */

	return 0;
}

PUBLIC void task_mm()
{
	while (1) {

		send_recv(RECEIVE, ANY, &mm_msg);

		switch (mm_msg.type) {
		case FORK:
			mm_msg.RETVAL = do_fork();
			break;
		default:
			kprintf("Invalid message type in task_mm!");
			break;
		}

		send_recv(SEND, mm_msg.source, &mm_msg);
	}
}
