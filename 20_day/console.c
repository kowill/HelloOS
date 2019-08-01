#include <stdio.h>
#include <string.h>
#include "bootpack.h"

void cons_newline(struct CONSOLE *cons)
{
    int x, y;
    struct SHEET *sheet = cons->sht;
    if (cons->cur_y < 28 + 112)
    {
        cons->cur_y += 16;
    }
    else
    {
        for (y = 28; y < 28 + 112; y++)
            for (x = 8; x < 8 + 240; x++)
                sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
        for (y = 28 + 112; y < 28 + 128; y++)
            for (x = 8; x < 8 + 240; x++)
                sheet->buf[x + y * sheet->bxsize] = COL8_000000;
        sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
    }
    cons->cur_x = 8;
    return;
}

void console_task(struct SHEET *sheet, unsigned int memtotal)
{
    struct TIMER *timer;
    struct TASK *task = task_now();
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    struct CONSOLE cons;
    int i, fifobuf[128], *fat = (int *)memman_alloc_4k(memman, 4 * 2880);
    char cmdline[30];
    cons.sht = sheet;
    cons.cur_x = 8;
    cons.cur_y = 28;
    cons.cur_c = -1;
    *((int *)0x0fec) = (int)&cons;

    fifo32_init(&task->fifo, 128, fifobuf, task);
    timer = timer_alloc();
    timer_init(timer, &task->fifo, 1);
    timer_settime(timer, 50);

    file_readfat(fat, (unsigned char *)(ADR_DISKING + 0x000200));

    cons_putchar(&cons, '>', 1);

    for (;;)
    {
        io_cli();
        if (fifo32_status(&task->fifo) == 0)
        {
            task_sleep(task);
            io_sti();
        }
        else
        {
            i = fifo32_get(&task->fifo);
            io_sti();
            if (i <= 1)
            {
                if (i != 0)
                {
                    timer_init(timer, &task->fifo, 0);
                    if (cons.cur_c >= 0)
                        cons.cur_c = COL8_FFFFFF;
                }
                else
                {
                    timer_init(timer, &task->fifo, 1);
                    if (cons.cur_c >= 0)
                        cons.cur_c = COL8_000000;
                }
                timer_settime(timer, 50);
            }
            if (i == 2)
                cons.cur_c = COL8_FFFFFF;
            if (i == 3)
            {
                boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, 43);
                cons.cur_c = -1;
            }
            if (256 <= i && i <= 511)
            {
                if (i == 8 + 256)
                {
                    if (cons.cur_x > 16)
                    {
                        cons_putchar(&cons, ' ', 0);
                        cons.cur_x -= 8;
                    }
                }
                // enter
                else if (i == 10 + 256)
                {
                    cons_putchar(&cons, ' ', 0);
                    cmdline[cons.cur_x / 8 - 2] = 0;
                    cons_newline(&cons);
                    cons_runcmd(cmdline, &cons, fat, memtotal);
                    cons_putchar(&cons, '>', 1);
                }
                else
                {
                    if (cons.cur_x < 240)
                    {
                        cmdline[cons.cur_x / 8 - 2] = i - 256;
                        cons_putchar(&cons, i - 256, 1);
                    }
                }
            }
            if (cons.cur_c >= 0)
                boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
            sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
        }
    }
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
    char s[2];
    s[0] = chr;
    s[1] = 0;
    if (s[0] == 0x09) // tab
    {
        for (;;)
        {
            putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
            cons->cur_x += 8;
            if (cons->cur_x == 8 + 240)
            {
                cons_newline(cons);
            }
            if (((cons->cur_x - 8) & 0x1f) == 0) // mod 32 = 0
                break;
        }
    }
    else if (s[0] == 0x0a) // cr
    {
        cons_newline(cons);
    }
    else if (s[0] == 0x0d) // lf
    {
    }
    else
    {
        putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
        if (move != 0)
        {
            cons->cur_x += 8;
            if (cons->cur_x == 8 + 240)
            {
                cons_newline(cons);
            }
        }
    }
    return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal)
{
    if (strcmp(cmdline, "mem") == 0)
        cmd_mem(cons, memtotal);
    else if (strcmp(cmdline, "cls") == 0)
        cmd_cls(cons);
    else if (strcmp(cmdline, "dir") == 0)
        cmd_dir(cons);
    else if (strncmp(cmdline, "type ", 5) == 0)
        cmd_type(cons, fat, cmdline);
    else if (cmdline[0] != 0)
    {
        if (cmd_app(cons, fat, cmdline) == 0)
        {
            putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
            cons_newline(cons);
            cons_newline(cons);
        }
    }
    return;
}

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal)
{
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    char s[30];

    sprintf(s, "total %dMB", memtotal / (1024 * 1024));
    putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
    cons_newline(cons);
    sprintf(s, "free %dKB", memman_total(memman) / 1024);
    putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
    cons_newline(cons);
    cons_newline(cons);
    return;
}

void cmd_cls(struct CONSOLE *cons)
{
    int x, y;
    for (y = 28; y < 28 + 128; y++)
        for (x = 8; x < 8 + 240; x++)
            cons->sht->buf[x + y * cons->sht->bxsize] = COL8_000000;
    sheet_refresh(cons->sht, 8, 28, 8 + 240, 29 + 128);
    cons->cur_y = 28;
    return;
}

void cmd_dir(struct CONSOLE *cons)
{
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKING + 0x002600);
    int x, y;
    char s[30];
    for (x = 0; x < 224; x++)
    {
        if (finfo[x].name[0] == 0x00)
            break;
        if (finfo[x].name[0] != 0xe5)
        {
            if ((finfo[x].type & 0x18) == 0)
            {
                sprintf(s, "filename.ext %7d", finfo[x].size);
                for (y = 0; y < 8; y++)
                    s[y] = finfo[x].name[y];
                s[9] = finfo[x].ext[0];
                s[10] = finfo[x].ext[1];
                s[11] = finfo[x].ext[2];
                putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
                cons_newline(cons);
            }
        }
    }
    cons_newline(cons);
    return;
}

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline)
{
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *)(ADR_DISKING + 0x002600), 224);
    char *p;
    int i;

    if (finfo != 0)
    {
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKING + 0x003e00));
        for (i = 0; i < finfo->size; i++)
            cons_putchar(cons, p[i], 1);
        memman_free_4k(memman, (int)p, finfo->size);
    }
    else
    {
        putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "file not dound.", 15);
        cons_newline(cons);
    }
    cons_newline(cons);
    return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    struct FILEINFO *finfo;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
    char name[18], *p;
    int i;

    for (i = 0; i < 13; i++)
    {
        if (cmdline[i] <= ' ')
            break;
        name[i] = cmdline[i];
    }
    name[i] = 0;

    finfo = file_search(name, (struct FILEINFO *)(ADR_DISKING + 0x002600), 224);
    if (finfo == 0 && name[i - 1] != '.')
    {
        name[i] = '.';
        name[i + 1] = 'H';
        name[i + 2] = 'R';
        name[i + 3] = 'B';
        name[i + 4] = 0;
        finfo = file_search(name, (struct FILEINFO *)(ADR_DISKING + 0x002600), 224);
    }
    if (finfo != 0)
    {
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKING + 0x003e00));
        set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
        farcall(0, 1003 * 8);
        memman_free_4k(memman, (int)p, finfo->size);
        cons_newline(cons);
        return 1;
    }
    return 0;
}
