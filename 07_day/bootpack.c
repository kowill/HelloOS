#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo;

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
    char s[20], mousecursor[256], keybuf[32];
    int i;

    init_gdtidt();
    init_pic();
    io_sti();

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    init_mouse_coursor8(mousecursor, COL8_008484);

    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "HELLO, WORLD!!");

    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    fifo8_init(&keyfifo, sizeof(keybuf), keybuf);

    for (;;)
    {
        io_cli();
        if (fifo8_status(&keyfifo) == 0)
        {
            io_stihlt();
        }
        else
        {
            i = fifo8_get(&keyfifo);
            io_sti();
            sprintf(s, "%02X", i);
            boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
            putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
        }
    }
}
