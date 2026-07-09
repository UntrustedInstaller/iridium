#include "kernel.h"
#include "shell.h"
#include "terminal.h"
#include "pit.h"
#include "keyboard.h"

#define SHELL_BUF 256
#define HISTORY_SIZE 8

static char history[HISTORY_SIZE][SHELL_BUF];
static int history_count = 0;

static void history_add(const char* line) {
    int len = strlen(line);
    if (len == 0) return;
    if (history_count >= HISTORY_SIZE) {
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            int j;
            for (j = 0; history[i + 1][j]; j++) history[i][j] = history[i + 1][j];
            history[i][j] = '\0';
        }
        history_count = HISTORY_SIZE - 1;
    }
    int i = 0;
    for (; i < len && i < SHELL_BUF - 1; i++) history[history_count][i] = line[i];
    history[history_count][i] = '\0';
    history_count++;
}

static void cmd_help(void) {
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

    terminal_write("Uptime: ");
    char buf[32];
    int p = 0;

    if (hours > 0) {
        if (hours > 99) { buf[p++] = '0' + hours / 100; }
        if (hours > 9)  { buf[p++] = '0' + (hours / 10) % 10; }
        buf[p++] = '0' + hours % 10;
        buf[p++] = ':';
    }
    buf[p++] = '0' + mins / 10;
    buf[p++] = '0' + mins % 10;
    buf[p++] = ':';
    buf[p++] = '0' + secs / 10;
    buf[p++] = '0' + secs % 10;
    buf[p] = '\0';
    terminal_write(buf);
    terminal_write("\n");
}

static void cmd_echo(const char* line) {
    const char* arg = line + 4;
    while (*arg == ' ') arg++;
    terminal_write(arg);
    terminal_write("\n");
}

static void cmd_version(void) {
    terminal_write("IridiumOS 32-bit Phase 1\n");
    terminal_write("Boot protocol: Multiboot + PVH ELF note\n");
    terminal_write("CPU: i686 (32-bit protected mode)\n");
}

void shell_run(void) {
    char buf[SHELL_BUF];
    int pos = 0;
    int history_pos = -1;

    terminal_setcolor(vga_entry_color(VGA_WHITE, VGA_BLUE));
    terminal_write("> ");

    while (1) {
        int c = keyboard_getchar();

        if (c == '\n') {
            terminal_write("\n");
            buf[pos] = '\0';

            if (pos > 0) {
                history_add(buf);
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
            history_pos = -1;
            pos = 0;
            terminal_write("> ");
        } else if (c == '\b' || c == 0x7F) {
            if (pos > 0) {
                pos--;
                terminal_putchar('\b');
                terminal_putchar(' ');
                terminal_putchar('\b');
            }
        } else if (c == KEY_UP) {
            if (history_count > 0 && history_pos < history_count - 1) {
                history_pos++;
                int idx = history_count - 1 - history_pos;
                int old_pos = pos;
                pos = 0;
                while (history[idx][pos]) pos++;
                terminal_putchar('\r');
                terminal_write("> ");
                terminal_write(history[idx]);
                for (int i = pos; i < old_pos; i++) terminal_putchar(' ');
                terminal_putchar('\r');
                terminal_write("> ");
                terminal_write(history[idx]);
                pos = strlen(history[idx]);
                int j = 0;
                while (history[idx][j]) { buf[j] = history[idx][j]; j++; }
                buf[j] = '\0';
            }
        } else if (c == KEY_DOWN) {
            if (history_pos > 0) {
                history_pos--;
                int idx = history_count - 1 - history_pos;
                int old_pos = pos;
                pos = 0;
                while (history[idx][pos]) pos++;
                terminal_putchar('\r');
                terminal_write("> ");
                terminal_write(history[idx]);
                for (int i = pos; i < old_pos; i++) terminal_putchar(' ');
                terminal_putchar('\r');
                terminal_write("> ");
                terminal_write(history[idx]);
                pos = strlen(history[idx]);
                int j = 0;
                while (history[idx][j]) { buf[j] = history[idx][j]; j++; }
                buf[j] = '\0';
            } else if (history_pos == 0) {
                history_pos = -1;
                int old_pos = pos;
                terminal_putchar('\r');
                terminal_write("> ");
                for (int i = 0; i < old_pos; i++) terminal_putchar(' ');
                terminal_putchar('\r');
                terminal_write("> ");
                pos = 0;
                buf[0] = '\0';
            }
        } else if (c >= 32 && c < 127) {
            if (pos < SHELL_BUF - 1) {
                buf[pos++] = c;
                terminal_putchar(c);
            }
        }
    }
}
