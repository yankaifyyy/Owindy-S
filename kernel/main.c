
//-----------------------------------------------------------------
//							main.c
//-----------------------------------------------------------------

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"
#include "global.h"
#include "kernel.h"
#include "util.h"
#include "tty.h"

PUBLIC TASK task_table[NR_TASKS] = {
    {task_sys, STACK_SIZE_SYS, "SYS"},
    //{task_tty, STACK_SIZE_TTY, "TTY"},
	{task_mm,  STACK_SIZE_MM,  "MM"}
};

PUBLIC TASK user_proc_table[NR_NATIVE_PROCS] = {
	{Init,  STACK_SIZE_INIT,  "Init"}, 
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"}
};

PUBLIC int kernel_main()
{
	kprintf("-----\"kernel_main\" begins-----\n");

	TASK *p_task;
	PROCESS *p_proc	= proc_table;

	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16_t selector_ldt = SELECTOR_LDT_FIRST;

	u8_t privilege, rpl;
	int eflags, priority;

	int i;
	for (i = 0; i < NR_TASK_PROCS; i++, p_proc++) {
		// 初始化 LDT 在 GDT 中的选择子
		p_proc->ldt_sel = selector_ldt;
		selector_ldt += 1 << 3;

		if (i >= NR_TASKS + NR_NATIVE_PROCS) {
			p_proc->p_flags = EMPTY;
			continue;
		}

		if (i < NR_TASKS) {
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; // IF = 1, IOPL = 1

			priority = 15;
		}
		else { 
			/* wind4869: 用户进程不能使用kprintf()
			 * yankai: 用户进程可以使用kprintf()
			 */
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; // 为了使用kprintf，让其运行在ring1

			priority = 5;
		}

		if (p_proc != proc_table + INIT) {
			/* 使用0~4G的扁平空间，只是改变一下特权级 */
			p_proc->ldts[INDEX_LDT_C]  = gdt[SELECTOR_KERNEL_CS >> 3];
			p_proc->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3],

			p_proc->ldts[INDEX_LDT_C].attr1 = DA_C | privilege << 5;
			p_proc->ldts[INDEX_LDT_RW].attr1 = DA_DRW | privilege << 5;
		}
		else {
			/* 假设Init也使用1M的内存空间，应该是够用的 */
			init_descriptor(&p_proc->ldts[INDEX_LDT_C],
					0,
					(PROC_DEFAULT_MEM - 1) >> LIMIT_4K_SHIFT,
					DA_32 | DA_LIMIT_4K | DA_C | priority << 5);

			init_descriptor(&p_proc->ldts[INDEX_LDT_RW],
					0,
					(PROC_DEFAULT_MEM - 1) >> LIMIT_4K_SHIFT,
					DA_32 | DA_LIMIT_4K | DA_DRW | priority << 5);
		}

		// 初始化段寄存器（选择子）
		p_proc->regs.cs	= INDEX_LDT_C << 3 | SA_TIL | rpl;

		p_proc->regs.ds	= 
		p_proc->regs.es	= 
		p_proc->regs.fs	= 
		p_proc->regs.ss	= INDEX_LDT_RW << 3 | SA_TIL | rpl;

		// 显存的描述符在gdt中
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		// 初始化进程的名字
		strcpy(p_proc->p_name, p_task->name);

		// eio, esp, eflags 的初始化
		p_proc->regs.eip = (u32_t)p_task->initial_eip;
		p_proc->regs.esp = (u32_t)p_task_stack;

		p_proc->regs.eflags = eflags;

		// 初始化时间片
		p_proc->ticks = p_proc->priority = priority;

		// 初始化消息传递的相关属性
		p_proc->p_flags = (i < 3) ? 0 : 1; // 屏蔽A，B，C
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		//p_proc->parent = NO_TASK;
		//p_proc->has_int_msg = 0;

		p_task_stack -= p_task->stacksize;
	}

	ticks = 0;
	k_reenter = 0;
	p_proc_ready = proc_table;

    init_clock();

	restart();

	while(1) {}
}

PUBLIC int get_ticks()
{
	MESSAGE msg;
	memset(&msg, 0, sizeof(MESSAGE));
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	
	return msg.RETVAL;
}

PUBLIC void delay(int time)
{
	int i, j, k;
	for (i = 0; i < time; i++)
		for (j = 0; j < 10; j++)
			for (k = 0; k < 100; k++)
				kprintf("");
}

PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

PUBLIC void Init()
{
	kprintf("\n<--------------------Init start-------------------->\n");

	int pid = fork();

	if (pid != 0) {
		kprintf("\n<----------parent is running, child pid:%d---------->\n", pid);
	} 
	else {
		kprintf("\n<------------------child is running----------------->\n");
	}

	while (1) {
		kprintf("<while:%d>", p_proc_ready - proc_table);
		delay(1);
	}
}

PUBLIC void TestA()
{
	while (1) {
		kprintf("<A>");
        //kprintf("<--A,ticks:%d-->", get_ticks());
		delay(1);
		//milli_delay(500);
	}
}

PUBLIC void TestB()
{
	while (1) {
        kprintf("<B>");
		delay(1);
		//milli_delay(200);
	}
}

PUBLIC void TestC()
{
	while (1) {
        kprintf("<C>");
		delay(1);
		//milli_delay(200);
	}
}

PUBLIC void task_sys()
{
    MESSAGE msg;
    while (1) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case GET_TICKS:
            msg.RETVAL = ticks;

            /*[>这句竟然会影响msg的值，去掉之后msg居然不正确了！！！！！<]*/
            kprintf("<task_sys,%d,%d,%d>", msg.type, msg.source, msg.RETVAL);

            send_recv(SEND, src, &msg);
            break;
        default:
            break;
        }
    }
}
