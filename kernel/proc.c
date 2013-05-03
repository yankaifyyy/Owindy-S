
//-----------------------------------------------------------
//							proc.c
//-----------------------------------------------------------
//										     wind4869, 2013/4
//-----------------------------------------------------------


#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "kernel.h"
#include "util.h"

PUBLIC system_call sys_call_table[NR_SYS_CALL] = {
	sys_get_ticks
}; 

PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;

	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks = p->priority;
			}
		}
	}

	disp_str("->");
}

PUBLIC void clock_handler(int irq)
{
	ticks++;
	/*p_proc_ready->ticks--;

	if (k_reenter != 0) {
		return;
	}

	if (p_proc_ready->ticks > 0) {
		return;
	}

	schedule();*/
	if (p_proc_ready == proc_table + 2) {
		p_proc_ready = proc_table;
	}
	else
		p_proc_ready++;
}

PUBLIC int sys_get_ticks()
{
	disp_str("#");
	return 0;
}
