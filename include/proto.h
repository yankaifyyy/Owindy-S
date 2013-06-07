#ifndef PROTO_H
#define PROTO_H

#include "type.h"
#include "proc.h"

// 系统调用
// As a micro kernel system, the number of syscall should be only one
// Here, write is only a temp syscall
#define INT_VECTOR_SYS_CALL 0x80
#define NR_SYS_CALL 2
// --------------------------

PUBLIC void clock_handler(int);
PUBLIC void keyboard_handler(int);

PUBLIC int sys_get_ticks();
PUBLIC int get_ticks();

PUBLIC int sys_write(char *buf, int len, PROCESS *p_proc);
PUBLIC void write(char *buf, int len);

#endif

