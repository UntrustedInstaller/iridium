#include "kernel.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "keyboard.h"
#include "shell.h"
#include "multiboot.h"
#include "pmm.h"

void kernel_main(void) {
    gdt_init();
    idt_init();
    pic_remap();
    pit_init();
    keyboard_init();

    __asm__("sti");

    terminal_initialize();

    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("IridiumOS 32-bit\n");
    terminal_write("Osmium's periodic neighbor\n\n");
    terminal_write("Pouring 0x0D cups of coffee... done.\n");
    terminal_write("Counting RAM... ");
    char buf[32];
    int len = 0;
    uint32_t mem = multiboot_get_total_memory_kb();
    if (mem >= 1024) {
        uint32_t mib = mem / 1024;
        if (mib >= 100) { buf[len++] = '0' + mib / 100; mib %= 100; }
        if (mib >= 10)  { buf[len++] = '0' + mib / 10; mib %= 10; }
        buf[len++] = '0' + mib;
        buf[len++] = 'M'; buf[len++] = 'i'; buf[len++] = 'B';
    } else {
        uint32_t n = mem;
        if (n >= 100) { buf[len++] = '0' + n / 100; n %= 100; }
        if (n >= 10)  { buf[len++] = '0' + n / 10; n %= 10; }
        buf[len++] = '0' + n;
        buf[len++] = 'K';
    }
    buf[len] = '\0';
    terminal_write(buf);
    terminal_write(" detected.\n");

    pmm_init();

    terminal_write("Physical frames: ");
    len = 0;
    uint32_t free = pmm_get_free_frames();
    if (free == 0) { buf[len++] = '0'; }
    else {
        while (free > 0) {
            char tmp[16];
            int j = 0;
            uint32_t n = free;
            if (n == 0) tmp[j++] = '0';
            while (n > 0) { tmp[j++] = '0' + n % 10; n /= 10; }
            while (j > 0) buf[len++] = tmp[--j];
            free = 0;
        }
    }
    buf[len++] = ' '; buf[len++] = 'f'; buf[len++] = 'r'; buf[len++] = 'e'; buf[len++] = 'e';
    buf[len] = '\0';
    terminal_write(buf);
    terminal_write("\n");

    terminal_write("Making sure the purple is purple enough. It is.\n");
    terminal_write("Spinning up the thing that spins up things... done.\n\n");

    shell_run();
}