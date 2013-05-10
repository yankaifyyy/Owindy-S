
//-----------------------------------------------------------------
//							main.c
//-----------------------------------------------------------------

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "kernel.h"
#include "util.h"

void TestA();
void TestB();
void TestC();

PUBLIC TASK	task_table[NR_TASKS] = {
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"}
};

PUBLIC int kernel_main()
{
	kprintf("-----\"kernel_main\" begins-----\n");
	
	TASK *p_task = task_table;
	PROCESS *p_proc	= proc_table;

	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16_t selector_ldt = SELECTOR_LDT_FIRST;

	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);
		p_proc->pid = i;

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32_t)p_task->initial_eip;
		p_proc->regs.esp = (u32_t)p_task_stack;
		p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1

		p_proc->ticks = p_proc->priority = 5;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 15;
	proc_table[1].ticks = proc_table[1].priority = 5;
	proc_table[2].ticks = proc_table[2].priority = 3;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

	// 初始化 8253 PIT 
	outb(TIMER_MODE, RATE_GENERATOR);
	outb(TIMER0, (u8_t)(TIMER_FREQ / HZ));
	outb(TIMER0, (u8_t)((TIMER_FREQ/HZ) >> 8));

    put_irq_handler(CLOCK_IRQ, clock_handler); // 设定时钟中断处理程序
    enable_irq(CLOCK_IRQ);                     // 让8259A可以接收时钟中断

	restart();

	while(1){}
}

PUBLIC void msleep(int ms)
{
	int t = get_ticks();
	while ((get_ticks() - t) * 10 < ms) {}
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
		//if (get_ticks())
			//kprintf("shot");
		delay(1);
		//msleep(500);
	}
}

PUBLIC void TestB()
{
	while(1){
		kprintf("B.");
		delay(1);
	}
}

PUBLIC void TestC()
{
	while(1){
		kprintf("C.");
		delay(1);
	}
}
