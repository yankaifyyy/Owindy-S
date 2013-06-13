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

	send_recv(BOTH, TASK_MM, &msg);

	return msg.PID;
}

