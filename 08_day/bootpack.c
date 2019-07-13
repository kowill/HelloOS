#include <stdio.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse(void);
void init_keyboard(void);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
    char s[20], mousecursor[256], keybuf[32], mousebuf[128];
    int i;
    unsigned char mouse_dbuf[8], mouse_phase;

    init_gdtidt();
    init_pic();
    io_sti();

    fifo8_init(&keyfifo, sizeof(keybuf), keybuf);
    fifo8_init(&mousefifo, sizeof(mousebuf), mousebuf);
    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    init_mouse_coursor8(mousecursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, (binfo->scrnx - 16) / 2, (binfo->scrny - 28 - 16) / 2, mousecursor, 16);

    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "HELLO, WORLD!!");

    init_keyboard();
    enable_mouse();

    mouse_phase = 0;

    for (;;)
    {
        io_cli();
        if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0)
        {
            io_stihlt();
        }
        else
        {
            if (fifo8_status(&keyfifo) != 0)
            {
                i = fifo8_get(&keyfifo);
                io_sti();
                sprintf(s, "%02X", i);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
            }
            else if (fifo8_status(&mousefifo) != 0)
            {
                i = fifo8_get(&mousefifo);
                io_sti();
                if (mouse_phase == 0)
                {
                    if (i == 0xfa)
                        mouse_phase = 1;
                }
                else if (mouse_phase == 1)
                {
                    mouse_dbuf[0] = i;
                    mouse_phase = 2;
                }
                else if (mouse_phase == 2)
                {
                    mouse_dbuf[1] = i;
                    mouse_phase = 3;
                }
                else if (mouse_phase == 3)
                {
                    mouse_dbuf[2] = i;
                    mouse_phase = 1;

                    sprintf(s, "%02X %02X %02X", mouse_dbuf[0], mouse_dbuf[1], mouse_dbuf[2]);
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 8 * 8 - 1, 31);
                    putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
                }
            }
        }
    }
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

void init_keyboard(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

void enable_mouse(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    return;
}
