#include "tty.h"

PRIVATE void init_tty(TTY *p_tty);
PRIVATE void tty_do_read(TTY *p_tty);
PRIVATE void tty_do_write(TTY *p_tty);
PRIVATE void put_key(TTY *p_tty, u32_t key);

PUBLIC void task_tty()
{
    TTY *p_tty = &tty0;

    init_keyboard();

    init_tty(p_tty);

    while (1) {
        tty_do_read(p_tty);
        tty_do_write(p_tty);
    }
}

PRIVATE void tty_do_read(TTY *p_tty)
{
    keyboard_read(p_tty);
}

PRIVATE void tty_do_write(TTY *p_tty)
{
    if (p_tty->inbuf_count) {
        char ch = *(p_tty->p_inbuf_tail);
        p_tty->p_inbuf_tail++;
        if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
            p_tty->p_inbuf_tail = p_tty->in_buf;
        }
        p_tty->inbuf_count--;

        kputchar(ch);
    }
}

PUBLIC void tty_write(TTY *p_tty, char *buf, int len)
{
    char *p = buf;
    int i = len;
    while (i) {
        kputchar(*p++);
        --i;
    }
}

PUBLIC void in_process(TTY* p_tty, u32_t key)
{
    if (!(key & FLAG_EXT)) {
        put_key(p_tty, key);
    } else {
        int raw_code = key & MASK_RAW;
        switch (raw_code) {
        case ENTER:
            put_key(p_tty, '\n');
            break;
        case BACKSPACE:
            put_key(p_tty, '\b');
            break;
        default:
            break;
        }
    }
}

PRIVATE void put_key(TTY* p_tty, u32_t key)
{
    if (p_tty->inbuf_count < TTY_IN_BYTES) {
        *(p_tty->p_inbuf_head) = key;
        p_tty->p_inbuf_head++;
        if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
            p_tty->p_inbuf_head = p_tty->in_buf;
        }
        p_tty->inbuf_count++;
    }
}

PRIVATE void init_tty(TTY *p_tty) {
    p_tty->inbuf_count = 0;
    p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;
}

PUBLIC int sys_write(char *buf, int len, PROCESS *p_proc)
{
    tty_write(&tty0, buf, len);
    return len;
}


