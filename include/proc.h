
//-------------------------------------------------------
//						proc.h
//-------------------------------------------------------

#ifndef	_OWINDYS_PROC_H_
#define	_OWINDYS_PROC_H_

// 一个极简的消息结构
typedef struct message {
	int source;
	int type;
	int retval;
} MESSAGE;

enum msgtype {
	GET_TICKS,
};

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

	u32_t pid;     
    int ticks;      
    int priority;
	char p_name[16];

	int p_flags;
	MESSAGE *p_msg;
	int p_recvfrom; // 要从该进程接收消息
	int p_sendto; // 要发送消息给该进程

	//int has_int_msg; // 不太了解有什么用???

	struct proc *q_sending; // 消息发送队列
	struct proc *next_sending;
} PROCESS;

typedef struct task {
	task_f  initial_eip;
	int		stacksize;
	char	name[32];
} TASK;

#endif
