#include "kernel.h"
#include "shell.h"
#include "terminal.h"
#include "pit.h"
#include "keyboard.h"

#define SHELL_BUF 256

static void cmd_help(void) {
    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("Available commands:\n");
    terminal_write("  help    - Show this help\n");
    terminal_write("  clear   - Clear screen\n");
    terminal_write("  uptime  - Show system uptime\n");
    terminal_write("  echo    - Echo text back\n");
    terminal_write("  version - Show kernel version\n");
}

static void cmd_uptime(void) {
    uint32_t t = pit_get_tick();
    uint32_t secs = t / 100;
    uint32_t mins = secs / 60;
    uint32_t hours = mins / 60;
    secs %= 60;
    mins %= 60;

    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("Uptime: ");
    char buf[32];
    int pos = 0;

    if (hours > 0) {
        if (hours > 99) { buf[pos++] = '0' + hours / 100; }
        if (hours > 9)  { buf[pos++] = '0' + (hours / 10) % 10; }
        buf[pos++] = '0' + hours % 10;
        buf[pos++] = ':';
    }
    buf[pos++] = '0' + mins / 10;
    buf[pos++] = '0' + mins % 10;
    buf[pos++] = ':';
    buf[pos++] = '0' + secs / 10;
    buf[pos++] = '0' + secs % 10;
    buf[pos] = '\0';
    terminal_write(buf);
    terminal_write("\n");
}

static void cmd_echo(const char* line) {
    const char* arg = line + 4;
    while (*arg == ' ') arg++;
    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write(arg);
    terminal_write("\n");
}

static void cmd_version(void) {
    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("IridiumOS 32-bit Phase 1\n");
    terminal_write("Boot protocol: Multiboot + PVH ELF note\n");
    terminal_write("CPU: i686 (32-bit protected mode)\n");
}

void shell_run(void) {
    char buf[SHELL_BUF];
    int pos = 0;

    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("> ");

    while (1) {
        char c = keyboard_getchar();

        if (c == '\n') {
            terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
            terminal_write("\n");
            buf[pos] = '\0';

            if (pos > 0) {
                if (strncmp(buf, "help", 4) == 0 && (buf[4] == '\0' || buf[4] == ' ')) {
                    cmd_help();
                } else if (strncmp(buf, "clear", 5) == 0) {
                    terminal_clear();
                } else if (strncmp(buf, "uptime", 6) == 0) {
                    cmd_uptime();
                } else if (strncmp(buf, "echo", 4) == 0) {
                    cmd_echo(buf);
                } else if (strncmp(buf, "version", 7) == 0) {
                    cmd_version();
                } else {
                    terminal_write("Unknown command. Type 'help'.\n");
                }
            }

            terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
            terminal_write("> ");
            pos = 0;
        } else if (c == '\b' || c == 0x7F) {
            if (pos > 0) {
                pos--;
                terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
                terminal_putchar('\b');
                terminal_putchar(' ');
                terminal_putchar('\b');
            }
        } else {
            buf[pos++] = c;
            terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
            terminal_putchar(c);
        }
    }
}
