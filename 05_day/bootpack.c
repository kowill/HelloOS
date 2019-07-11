#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

struct BOOTINFO
{
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};

void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

void show_boxes();
void init_screen(char *vram, int xsize, int ysize);

void HariMain(void)
{
    struct BOOTINFO *binfo;

    binfo = (struct BOOTINFO *)0x0ff0;

    init_palette();

    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

    for (;;)
        io_hlt();
}

void init_screen(char *vram, int xsize, int ysize)
{
    boxfill8(vram, xsize, COL8_008484, 0, 0, xsize - 1, ysize - 29);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
    boxfill8(vram, xsize, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

    boxfill8(vram, xsize, COL8_FFFFFF, 3, ysize - 24, 59, ysize - 24);
    boxfill8(vram, xsize, COL8_FFFFFF, 2, ysize - 24, 2, ysize - 4);
    boxfill8(vram, xsize, COL8_848484, 3, ysize - 4, 59, ysize - 4);
    boxfill8(vram, xsize, COL8_848484, 59, ysize - 23, 59, ysize - 5);
    boxfill8(vram, xsize, COL8_000000, 2, ysize - 3, 59, ysize - 3);
    boxfill8(vram, xsize, COL8_000000, 60, ysize - 24, 60, ysize - 3);

    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize - 4, ysize - 24);
    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize - 4);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize - 3, xsize - 4, ysize - 3);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 3, ysize - 24, xsize - 3, ysize - 3);
}

void show_boxes()
{
    char *p = (char *)0xa0000;

    boxfill8(p, 320, COL8_FF0000, 20, 20, 120, 120);
    boxfill8(p, 320, COL8_00FF00, 70, 50, 170, 150);
    boxfill8(p, 320, COL8_0000FF, 120, 80, 220, 180);
}
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
    int x, y;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            vram[y * xsize + x] = c;
    return;
}
void init_palette(void)
{
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00, /* 0: black */
        0xff, 0x00, 0x00, /* 1: bright read */
        0x00, 0xff, 0x00, /* 2: bright green */
        0xff, 0xff, 0x00, /* 3: bright yellow */
        0x00, 0x00, 0xff, /* 4: bright blue */
        0xff, 0x00, 0xff, /* 5: bright purple */
        0x00, 0xff, 0xff, /* 6: bright light blue */
        0xff, 0xff, 0xff, /* 7: white */
        0xc6, 0xc6, 0xc6, /* 8: light gray */
        0x84, 0x00, 0x00, /* 9: bark red */
        0x00, 0x84, 0x00, /* 10: bark green */
        0x84, 0x84, 0x00, /* 11: bark yellow */
        0x00, 0x00, 0x84, /* 12: dark blue */
        0x84, 0x00, 0x84, /* 13: dark purple */
        0x00, 0x84, 0x84, /* 14: dark light blue */
        0x84, 0x84, 0x84, /* 15: dark gray */
    };
    set_palette(0, 15, table_rgb);
    return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
    int i, eflags;
    eflags = io_load_eflags(); /* 割り込み許可フラグ */
    io_cli();                  /* 割り込み禁止 */
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++)
    {
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags); /* 割り込み許可フラグ */
    return;
}
