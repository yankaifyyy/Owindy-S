
//-----------------------------------------------------------------
//							main.c
//-----------------------------------------------------------------
//												   wind4869, 2013/4
//-----------------------------------------------------------------

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "kernel.h"
#include "util.h"

void get_ticks();
void TestA();
void TestB();
void TestC();

PUBLIC TASK	task_table[NR_TASKS] = {
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"}
};

TASK* p_task = task_table;
PROCESS* p_proc	= proc_table;
char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
u16_t	selector_ldt = SELECTOR_LDT_FIRST;

PUBLIC int kernel_main()
{
	kprintf("-----\"kernel_main\" begins-----\n");
	
	//loop_counter = 0;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		/* code */
	
	//while (loop_counter < NR_TASKS) {
		strcpy(p_proc->p_name, p_task->name);
		//p_proc->pid = i;

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

		//loop_counter++;
	}
/*
	proc_table[0].ticks = proc_table[0].priority = 15;
	proc_table[1].ticks = proc_table[1].priority = 5;
	proc_table[2].ticks = proc_table[2].priority = 3;
*/
	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

    put_irq_handler(CLOCK_IRQ, clock_handler); // 设定时钟中断处理程序
    enable_irq(CLOCK_IRQ);                     // 让8259A可以接收时钟中断

	restart();

	while(1){}
}

void delay(int time)
{
	for (i = 0; i < time; i++) {
		for (j = 0; j < 10; j++) {
			for (k = 0; k < 10000; k++) {
				kprintf("");
			}
		}
	}
}

void TestA()
{
	while (1) {
		kprintf("A.");
		get_ticks();
		delay(1);
	}
}

void TestB()
{
	while(1){
		kprintf("B.");
		get_ticks();
		delay(1);
	}
}

void TestC()
{
	while(1){
		kprintf("C.");
		get_ticks();
		delay(1);
	}
}
