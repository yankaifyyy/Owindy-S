#include "tty.h"

PUBLIC void task_tty()
{
    while (1) {
        keyboard_read();
    }
}
