__asm__(".code16gcc\n");
#include "apps.h"

#define VGA      ((volatile uint16_t*)0xB8000)
#define CELL(c,a) ((uint16_t)(((a) << 8) | (uint8_t)(c)))
#define ATTR(f,b) ((uint8_t)((f) | ((b) << 4)))

static void wait_ms(uint32_t ms) {
    uint32_t us = ms * 1000;
    __asm__ __volatile__ (
        "movb $0x86, %%ah\n\tint $0x15"
        : : "c"((uint16_t)(us >> 16)), "d"((uint16_t)(us & 0xFFFF))
        : "eax"
    );
}

static int kbhit(void) {
    uint8_t ok;
    __asm__ __volatile__ (
        "movb $0x01, %%ah\n\tint $0x16\n\t"
        "setnz %0"
        : "=q"(ok)
        : : "eax"
    );
    return ok;
}

static void rdkey(void) {
    __asm__ __volatile__("movb $0x00, %%ah\n\tint $0x16" : : : "eax");
}

static void vga_puts(int x, int y, const char* s, uint8_t attr) {
    while (*s)
        VGA[y * 80 + x++] = CELL(*s++, attr);
}

static void draw_border(uint8_t fg) {
    int x, y;
    uint8_t a = ATTR(fg, 0);
    VGA[0]    = CELL(0xDA, a);
    VGA[79]   = CELL(0xBF, a);
    VGA[1920] = CELL(0xC0, a);
    VGA[1999] = CELL(0xD9, a);
    for (x = 1; x < 79; x++) {
        VGA[x]      = CELL(0xC4, a);
        VGA[1920+x] = CELL(0xC4, a);
    }
    for (y = 1; y < 24; y++) {
        VGA[y*80]    = CELL(0xB3, a);
        VGA[y*80+79] = CELL(0xB3, a);
    }
}

void cmd_about(const char* args) {
    int y, x, i, tick = 0;
    uint16_t ram_kb;
    uint8_t pulse;
    int si, prev;
    static const char spinner[] = "|/-\\";
    static const uint8_t bcols[] = {9,11,13,15,14,12,10,8};
    uint8_t r, c;

    get_cursor_rc(&r, &c);

    for (y = 0; y < 25; y++)
        for (x = 0; x < 80; x++)
            VGA[y*80 + x] = CELL(' ', ATTR(0, 0));

    draw_border(bcols[0]);

    static const char title[] = "IRIDIUM OS";
    static const uint8_t tcols[] = {
        ATTR(12,0), ATTR(14,0), ATTR(10,0), ATTR(11,0), ATTR(9,0),
        ATTR(13,0), ATTR(15,0), ATTR(0,0),  ATTR(12,0), ATTR(14,0),
    };
    for (i = 0; i < 10; i++)
        VGA[2*80 + 34 + i] = CELL(title[i], tcols[i]);

    for (i = 0; i < 10; i++)
        VGA[3*80 + 34 + i] = CELL(0xC4, ATTR(8, 0));

    static const char* labels[] = {"Version","Kernel","CPU","RAM","Build"};
    for (i = 0; i < 5; i++) {
        vga_puts(20, 6 + i, labels[i], ATTR(14, 0));
        VGA[(6+i)*80 + 27] = CELL(':', ATTR(15, 0));
    }

    vga_puts(29, 6, "Migration Milestone 3+",    ATTR(15, 0));
    vga_puts(29, 7, "33 KB  (65 sectors, FAT12)", ATTR(15, 0));

    uint32_t has_cpuid = 0;
    __asm__ __volatile__ (
        "pushfl\n\t"
        "popl %%eax\n\t"
        "movl %%eax, %%ebx\n\t"
        "xorl %1, %%eax\n\t"
        "pushl %%eax\n\t"
        "popfl\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "cmpl %%ebx, %%eax\n\t"
        "je 1f\n\t"
        "movl $1, %0\n\t"
        "1:\n\t"
        : "=r"(has_cpuid)
        : "ir"(0x200000)
        : "eax", "ebx", "cc"
    );

    if (has_cpuid) {
        uint32_t max, ebx, ecx, edx;
        __asm__ __volatile__ (
            "xorl %%eax, %%eax\n\tcpuid"
            : "=a"(max), "=b"(ebx), "=c"(ecx), "=d"(edx)
        );
        char v[13];
        v[0]  =  ebx & 0xFF; v[1]  = (ebx >> 8) & 0xFF;
        v[2]  = (ebx >> 16) & 0xFF; v[3]  = (ebx >> 24) & 0xFF;
        v[4]  =  edx & 0xFF; v[5]  = (edx >> 8) & 0xFF;
        v[6]  = (edx >> 16) & 0xFF; v[7]  = (edx >> 24) & 0xFF;
        v[8]  =  ecx & 0xFF; v[9]  = (ecx >> 8) & 0xFF;
        v[10] = (ecx >> 16) & 0xFF; v[11] = (ecx >> 24) & 0xFF;
        v[12] = '\0';

        for (x = 29; x < 70; x++)
            VGA[8*80 + x] = CELL(' ', ATTR(0, 0));
        vga_puts(29, 8, v, ATTR(15, 0));
    } else {
        vga_puts(29, 8, "(CPUID unavailable)", ATTR(8, 0));
    }

    __asm__ __volatile__("int $0x12" : "=a"(ram_kb));
    for (x = 29; x < 50; x++)
        VGA[9*80 + x] = CELL(' ', ATTR(0, 0));
    {
        char rbuf[8];
        int ri = 0;
        if (ram_kb == 0) {
            rbuf[ri++] = '0';
        } else {
            uint16_t rt = ram_kb;
            char rd[6];
            int rdi = 0;
            while (rt > 0) {
                rd[rdi++] = '0' + (rt % 10);
                rt /= 10;
            }
            while (rdi > 0) rbuf[ri++] = rd[--rdi];
        }
        rbuf[ri++] = ' ';
        rbuf[ri++] = 'K';
        rbuf[ri++] = 'B';
        rbuf[ri] = '\0';
        vga_puts(29, 9, rbuf, ATTR(15, 0));
    }

    vga_puts(29, 10, __DATE__, ATTR(15, 0));
    VGA[10*80 + 45] = CELL(' ', ATTR(15, 0));
    vga_puts(46, 10, __TIME__, ATTR(8, 0));

    for (x = 20; x < 60; x++)
        VGA[12*80 + x] = CELL(0xC4, ATTR(8, 0));

    VGA[14*80 + 31] = CELL(0x10, ATTR(15, 0));
    VGA[14*80 + 32] = CELL(' ',  ATTR(15, 0));
    vga_puts(33, 14, "Press any key", ATTR(15, 0));
    VGA[14*80 + 47] = CELL(' ',  ATTR(15, 0));
    VGA[14*80 + 48] = CELL(0x11, ATTR(15, 0));

    VGA[5*80 + 18] = CELL(0x10, ATTR(8, 0));
    VGA[1*80 + 76] = CELL('|', ATTR(14, 0));

    while (!kbhit()) {
        int bi = tick & 7;
        draw_border(bcols[bi]);

        VGA[1*80 + 76] = CELL(spinner[tick & 3], ATTR(14, 0));

        pulse = (tick & 4) ? ATTR(15,0) : ATTR(8,0);
        VGA[14*80 + 31] = CELL(0x10, pulse);
        VGA[14*80 + 32] = CELL(' ',  pulse);
        vga_puts(33, 14, "Press any key", pulse);
        VGA[14*80 + 47] = CELL(' ',  pulse);
        VGA[14*80 + 48] = CELL(0x11, pulse);

        si = (tick >> 3) % 5;
        prev = (si + 4) % 5;
        VGA[(5+prev)*80 + 18] = CELL(0x10, ATTR(8, 0));
        VGA[(5+si)*80 + 18] = CELL(0x10, ATTR(14, 0));

        wait_ms(180);
        tick++;
    }

    rdkey();
    gotoxy(0, r);
}
