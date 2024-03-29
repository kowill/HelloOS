#include "bootpack.h"

struct FIFO8 keyfifo, mousefifo;

void init_pic(void)
{
    io_out8(PIC0_IMR, 0xff); /* 割り込み受け付けない */
    io_out8(PIC1_IMR, 0xff); /* 割り込み受け付けない */

    io_out8(PIC0_ICW1, 0x11);   /* エッジトリガ */
    io_out8(PIC0_ICW2, 0x20);   /* IRQ 0-7 を INT 20-27 で受ける */
    io_out8(PIC0_ICW3, 1 << 2); /* PIC1 <-> IRQ2 */
    io_out8(PIC0_ICW4, 0x01);   /* non buffer */

    io_out8(PIC1_ICW1, 0x11); /* エッジトリガ */
    io_out8(PIC1_ICW2, 0x28); /* IRQ 8-15 を INT 28-2f で受ける */
    io_out8(PIC1_ICW3, 2);    /* PIC1 <-> IRQ2 */
    io_out8(PIC1_ICW4, 0x01); /* non buffer */

    io_out8(PIC0_IMR, 0xfb); /* PIC1以外は禁止 (11111011) */
    io_out8(PIC1_IMR, 0xff); /* 割り込み受け付けない */

    return;
}

void inthandler21(int *esp)
{
    /* from keybord */
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61); // IRQ-01受付完了
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo, data);
    return;
}

void inthandler2c(int *esp)
{
    /* from mouse */
    unsigned char data;
    io_out8(PIC1_OCW2, 0x64); // IRQ-12受付完了
    io_out8(PIC0_OCW2, 0x62); // IRQ-02受付完了
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&mousefifo, data);
    return;
}
