//
//-----------------------------------------------------------------
//                            fork.c
//-----------------------------------------------------------------
//

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"
#include "global.h"
#include "util.h"

PUBLIC int fork() 
{
	MESSAGE msg;
	msg.type = FORK;

	kprintf("<fork>"); // 诡异kprint，修正TASK_MM
	send_recv(BOTH, TASK_MM, &msg);

	kprintf("\n<fork:%d,%d>\n", p_proc_ready - proc_table, msg.PID);
	return msg.PID;
}

