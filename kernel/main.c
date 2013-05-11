
//-----------------------------------------------------------------
//							main.c
//-----------------------------------------------------------------

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "kernel.h"
#include "util.h"

PUBLIC TASK task_table[NR_TASKS] = {
	{task_sys, STACK_SIZE_SYS, "SYS"}
};

PUBLIC TASK user_proc_table[NR_PROCS] = {
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
	for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
		if (i < NR_TASKS) {
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; // IF = 1, IOPL = 1

			priority = 15;
		}
		else {
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; // IF = 1

			priority = 15;
		}

		// 初始化进程号和进程名
		p_proc->pid = i;
		strcpy(p_proc->p_name, p_task->name);

		// 初始化ldt及其选择子
		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		
		// 初始化段寄存器（段选择子）
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| rpl;

		p_proc->regs.eip = (u32_t)p_task->initial_eip;
		p_proc->regs.esp = (u32_t)p_task_stack;
		p_proc->regs.eflags = eflags;

		p_proc->ticks = p_proc->priority = priority;

		// 初始化消息传递相关属性
		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		p_task_stack -= p_task->stacksize;

		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	ticks = 0;
	k_reenter = 0;
	p_proc_ready = proc_table;

	// 初始化 8253 PIT 
	outb(TIMER_MODE, RATE_GENERATOR);
	outb(TIMER0, (u8_t)(TIMER_FREQ / HZ));
	outb(TIMER0, (u8_t)((TIMER_FREQ / HZ) >> 8));

    put_irq_handler(CLOCK_IRQ, clock_handler); // 设定时钟中断处理程序
    enable_irq(CLOCK_IRQ);                     // 让8259A可以接收时钟中断

	restart();

	while(1) {}
}

void delay(int time)
{
	int i, j, k;
	for (i = 0; i < time; i++)
		for (j = 0; j < 10; j++)
			for (k = 0; k < 100; k++)
				kprintf("");
}

PUBLIC void TestA()
{
	while (1) {
		kprintf("A.");
		delay(1);
	}
}

PUBLIC void TestB()
{
	while (1) {
		kprintf("B.");
		delay(1);
	}
}

PUBLIC void TestC()
{
	while (1) {
		kprintf("C.");
		delay(1);
	}
}

PUBLIC void task_sys()
{
	while (1) {
		kprintf("SYS.");
		delay(1);
	}
}
