__asm__(".code16gcc\n");
#include "apps.h"

extern uint8_t cur_col;
extern char _kernel_end[];

#define ATTR(f,b) ((uint8_t)((f) | ((b) << 4)))

static void wait_ms(uint32_t ms) {
    uint32_t us = ms * 1000;
    __asm__ __volatile__ (
        "movb $0x86, %%ah\n\tint $0x15"
        : : "c"((uint16_t)(us >> 16)), "d"((uint16_t)(us & 0xFFFF))
        : "eax"
    );
}

static void putc_at(uint8_t x, uint8_t y, char c, uint8_t attr) {
    gotoxy(x, y);
    cur_col = attr;
    print_char(c);
}

static void puts_at(uint8_t x, uint8_t y, const char* s, uint8_t attr) {
    gotoxy(x, y);
    cur_col = attr;
    print_str(s);
}

static void draw_border(uint8_t fg) {
    uint8_t a = ATTR(fg, 0);
    int i;
    putc_at(0, 0, 0xDA, a);
    putc_at(79, 0, 0xBF, a);
    putc_at(0, 24, 0xC0, a);
    gotoxy(79, 24);
    cur_col = a;
    __asm__ __volatile__ (
        "movb $0x09, %%ah\n\tmovb $0x00, %%bh\n\tmovw $1, %%cx\n\tint $0x10"
        : : "a"(0xD9), "b"((uint16_t)a) : "cc"
    );
    for (i = 1; i < 79; i++) {
        putc_at(i, 0, 0xC4, a);
        putc_at(i, 24, 0xC4, a);
    }
    for (i = 1; i < 24; i++) {
        gotoxy(0, i);
        cur_col = a;
        __asm__ __volatile__ (
            "movb $0x09, %%ah\n\tmovb $0x00, %%bh\n\tmovw $1, %%cx\n\tint $0x10"
            : : "a"(0xB3), "b"((uint16_t)a) : "cc"
        );
        gotoxy(79, i);
        __asm__ __volatile__ (
            "movb $0x09, %%ah\n\tmovb $0x00, %%bh\n\tmovw $1, %%cx\n\tint $0x10"
            : : "a"(0xB3), "b"((uint16_t)a) : "cc"
        );
    }
}

static int u16_to_str(char* buf, uint16_t n) {
    int i = 0;
    if (n == 0) { buf[i++] = '0'; return i; }
    char tmp[6];
    int ti = 0;
    while (n > 0) { tmp[ti++] = '0' + (n % 10); n /= 10; }
    while (ti > 0) buf[i++] = tmp[--ti];
    return i;
}

void cmd_about(const char* args) {
    int i, tick = 0;
    uint16_t ram_kb;
    uint8_t pulse;
    int si, prev;
    uint8_t r, c;
    uint8_t saved_col = cur_col;
    static const char spinner[] = "|/-\\";
    static const uint8_t bcols[] = {9,11,13,15,14,12,10,8};

    get_cursor_rc(&r, &c);

    __asm__ __volatile__ (
        "movb $0x01, %%ah\n\tmovw $0x2000, %%cx\n\tint $0x10"
        : : : "eax", "ecx"
    );

    cur_col = 0;
    clear_screen();

    draw_border(bcols[0]);

    puts_at(34, 2, "IRIDIUM OS", ATTR(15, 0));

    for (i = 0; i < 10; i++)
        putc_at(34 + i, 3, 0xC4, ATTR(8, 0));

    {
        const char* labels[] = {"Version", "Kernel", "CPU", "RAM", "Build"};
        for (i = 0; i < 5; i++) {
            puts_at(20, 6 + i, labels[i], ATTR(14, 0));
            putc_at(27, 6 + i, ':', ATTR(15, 0));
        }
    }

    puts_at(29, 6, "Migration Milestone 3+",    ATTR(15, 0));
    {
        uint16_t kb = (uint32_t)_kernel_end;
        uint16_t kk = (kb + 1023) / 1024;
        uint16_t ks = (kb + 511) / 512;
        char kbuf[26];
        int ki = 0;
        ki += u16_to_str(kbuf + ki, kk);
        kbuf[ki++] = ' '; kbuf[ki++] = 'K'; kbuf[ki++] = 'B';
        kbuf[ki++] = ' '; kbuf[ki++] = '(';
        ki += u16_to_str(kbuf + ki, ks);
        kbuf[ki++] = ' '; kbuf[ki++] = 's'; kbuf[ki++] = 'e';
        kbuf[ki++] = 'c'; kbuf[ki++] = 't'; kbuf[ki++] = 'o';
        kbuf[ki++] = 'r'; kbuf[ki++] = 's'; kbuf[ki++] = ',';
        kbuf[ki++] = ' '; kbuf[ki++] = 'F'; kbuf[ki++] = 'A';
        kbuf[ki++] = 'T'; kbuf[ki++] = '1'; kbuf[ki++] = '2';
        kbuf[ki] = '\0';
        puts_at(29, 7, kbuf, ATTR(15, 0));
    }

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

        puts_at(29, 8, v, ATTR(15, 0));
    } else {
        puts_at(29, 8, "(CPUID unavailable)", ATTR(8, 0));
    }

    ram_kb = get_mem_size();
    {
        char rbuf[8];
        int ri = u16_to_str(rbuf, ram_kb);
        rbuf[ri++] = ' ';
        rbuf[ri++] = 'K';
        rbuf[ri++] = 'B';
        rbuf[ri] = '\0';
        puts_at(29, 9, rbuf, ATTR(15, 0));
    }

    puts_at(29, 10, __DATE__, ATTR(15, 0));
    putc_at(45, 10, ' ', ATTR(15, 0));
    puts_at(46, 10, __TIME__, ATTR(8, 0));

    for (i = 20; i < 60; i++)
        putc_at(i, 12, 0xC4, ATTR(8, 0));

    putc_at(31, 14, 0x10, ATTR(15, 0));
    putc_at(32, 14, ' ',  ATTR(15, 0));
    puts_at(33, 14, "Press any key", ATTR(15, 0));
    putc_at(47, 14, ' ',  ATTR(15, 0));
    putc_at(48, 14, 0x11, ATTR(15, 0));

    putc_at(18, 6, 0x10, ATTR(8, 0));
    putc_at(76, 1, '|',  ATTR(14, 0));

    for (tick = 0; tick < 30; tick++) {
        int bi = tick & 7;
        draw_border(bcols[bi]);

        putc_at(76, 1, spinner[tick & 3], ATTR(14, 0));

        pulse = (tick & 4) ? ATTR(15,0) : ATTR(8,0);
        putc_at(31, 14, 0x10, pulse);
        putc_at(32, 14, ' ',  pulse);
        puts_at(33, 14, "Press any key", pulse);
        putc_at(47, 14, ' ',  pulse);
        putc_at(48, 14, 0x11, pulse);

        si = (tick >> 3) % 5;
        prev = (si + 4) % 5;
        putc_at(18, 6 + prev, 0x10, ATTR(8, 0));
        putc_at(18, 6 + si,  0x10, ATTR(14, 0));

        wait_ms(180);
    }

    cur_col = saved_col;
    clear_screen();

    __asm__ __volatile__ (
        "movb $0x01, %%ah\n\tmovw $0x0607, %%cx\n\tint $0x10"
        : : : "eax", "ecx"
    );
}
