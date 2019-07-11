#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;
    char s[20], mousecursor[256];

    init_gdtidt();
    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    init_mouse_coursor8(mousecursor, COL8_008484);

    putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "HELLO, WORLD!!");
    putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "Haribote OS.");
    putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Haribote OS.");

    sprintf(s, "scrnx = %d", binfo->scrnx);
    putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, (binfo->scrnx - 16) / 2, (binfo->scrny - 28 - 16) / 2, mousecursor, 16);

    for (;;)
        io_hlt();
}
