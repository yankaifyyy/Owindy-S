
//------------------------------------------------------
//					global.h
//------------------------------------------------------
//									wind4869, 2013/4/3
//------------------------------------------------------

#ifndef _OWINDYS_GLOBAL_H_
#define _OWINDYS_GLOBAL_H_ 

PUBLIC int disp_pos;
PUBLIC u8 gdt_ptr[6];//0~15:limit, 16~47:base
PUBLIC DESCRIPTOR gdt[GDT_SIZE];
PUBLIC u8 idt_ptr[6];
PUBLIC GATE idt[IDT_SIZE];

#endif

