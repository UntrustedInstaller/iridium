#include "kernel.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "keyboard.h"
#include "shell.h"

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
    terminal_write("Counting RAM... 639. 640. Wait. 639.\n");
    terminal_write("Making sure the purple is purple enough. It is.\n");
    terminal_write("Spinning up the thing that spins up things... done.\n\n");

    shell_run();
}
