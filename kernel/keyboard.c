#include "type.h"
#include "keyboard.h"
#include "kernel.h"

PRIVATE KB_INPUT    kb_in;

PRIVATE int code_with_E0;
PRIVATE int shift_l;
PRIVATE int shift_r;
PRIVATE int alt_l;
PRIVATE int alt_r;
PRIVATE int cltr_l;
PRIVATE int cltr_r;
PRIVATE bool caps_lock;
PRIVATE bool num_lock;
PRIVATE bool scroll_lock;
PRIVATE int column;

PRIVATE u8_t get_byte_from_kbuf();
PRIVATE void set_leds();
PRIVATE void kb_wait();
PRIVATE void kb_ack();

PUBLIC void keyboard_handler(int irq)
{
    u8_t scan_code = inb(0x60);

    if (kb_in.count < KB_IN_BYTES) {
        *(kb_in.p_head) = scan_code;
        kb_in.p_head++;
        if (kb_in.p_head == kb_in.buf + KB_IN_BYTES) {
            kb_in.p_head = kb_in.buf;
        }
        kb_in.count++;
    }
}

PUBLIC void init_keyboard()
{
    kb_in.count = 0;
    kb_in.p_head = kb_in.p_tail = kb_in.buf;

    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}


