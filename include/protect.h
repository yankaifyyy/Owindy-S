
//-------------------------------------------------------------
//						protect.h
//-------------------------------------------------------------
//											wind4869, 2013/4/2
//-------------------------------------------------------------

#ifndef _OWINDYS_PROTECT_H_
#define _OWINDYS_PROTECT_H_

//代码段/数据段描述符(8个字节)
typedef struct descriptor
{
	u8_t	limit_low;
	u16_t	base_low;
	u8_t	base_mid;
	u8_t	attr1;//P(1) DPL(2) S(1) TYPE(4)
	u8_t	limit_high_attr2;//G(1) D(1) 0(1) AVL(1) light_high(4)
	u8_t	base_high;
} DESCRIPTOR;

//门描述符
typedef struct gate
{
	u16_t offset_low;
	u16_t selector;
	u8_t param_count;
	u8_t attr;//P(1) DPL(2) S(1) TYPE(4)
	u16_t offset_high;
} GATE;

#define GDT_SIZE 128
#define IDT_SIZE 256

#define	PRIVILEGE_KRNL 0
#define PRIVILEGE_USER 3

#define SELECTOR_KERNEL_CS 8


//中断向量号
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

#define	DA_386IGate	0x8E//386 中断门类型值

#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT	0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT	0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG	0x9
#define	INT_VECTOR_INVAL_TSS	0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT	0xC
#define	INT_VECTOR_PROTECTION	0xD
#define	INT_VECTOR_PAGE_FAULT	0xE
#define	INT_VECTOR_COPROC_ERR	0x10

#endif
