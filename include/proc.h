
//-------------------------------------------------------
//						proc.h
//-------------------------------------------------------
//										wind4869, 2013/4
//-------------------------------------------------------

#ifndef	_OWINDYS_PROC_H_
#define	_OWINDYS_PROC_H_

typedef struct stackframe {
	u32_t	gs;
	u32_t	fs;
	u32_t	es;
	u32_t	ds;
	u32_t	edi;
	u32_t	esi;
	u32_t	ebp;
	u32_t	kernel_esp;
	u32_t	ebx;		
	u32_t	edx;
	u32_t	ecx;
	u32_t	eax;
	u32_t	retaddr;
	u32_t	eip;
	u32_t	cs;
	u32_t	eflags;
	u32_t	esp;
	u32_t	ss;	
} STACK_FRAME;

typedef struct proc {
	STACK_FRAME regs;     

	u16_t ldt_sel;    
	DESCRIPTOR ldts[LDT_SIZE];

    int ticks;      
    int priority;

	u32_t pid;     
	char p_name[16];
} PROCESS;

typedef struct task {
	task_f  initial_eip;
	int		stacksize;
	char	name[32];
} TASK;

#endif
