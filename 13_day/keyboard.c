#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp)
{
    /* from keybord */
    int data;
    io_out8(PIC0_OCW2, 0x61); // IRQ-01受付完了
    data = io_in8(PORT_KEYDAT);
    fifo32_put(keyfifo, data + keydata0);
    return;
}

void wait_KBC_sendready(void)
{
    /* wait until keybord controller is ready to send data */
    for (;;)
    {
        if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NORTEADY) == 0)
        {
            break;
        }
    }
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
    keyfifo = fifo;
    keydata0 = data0;

    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}
