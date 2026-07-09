#include "kernel.h"
#include "shell.h"
#include "terminal.h"
#include "pit.h"
#include "keyboard.h"
#include "pmm.h"

#define SHELL_BUF 256
#define HISTORY_SIZE 8

static char history[HISTORY_SIZE][SHELL_BUF];
static int history_count = 0;
static char hostname[64] = "iridium";

struct cmd {
    const char* name;
    const char* desc;
    void (*handler)(const char* args);
};

static void cmd_help(const char*);
static void cmd_clear(const char*);
static void cmd_echo(const char*);
static void cmd_uptime(const char*);
static void cmd_version(const char*);
static void cmd_uname(const char*);
static void cmd_whoami(const char*);
static void cmd_id(const char*);
static void cmd_hostname(const char*);
static void cmd_true(const char*);
static void cmd_false(const char*);
static void cmd_mem(const char*);

static const struct cmd commands[] = {
    {"help",    "Show this help",             cmd_help},
    {"clear",   "Clear screen",               cmd_clear},
    {"echo",    "Echo text back",             cmd_echo},
    {"uptime",  "Show system uptime",         cmd_uptime},
    {"version", "Show kernel version",        cmd_version},
    {"uname",   "Print system information",   cmd_uname},
    {"whoami",  "Print effective user ID",    cmd_whoami},
    {"id",      "Print user identity",        cmd_id},
    {"hostname","Print or set hostname",      cmd_hostname},
    {"true",    "Do nothing, successfully",   cmd_true},
    {"false",   "Do nothing, unsuccessfully", cmd_false},
    {"mem",     "Show memory usage",          cmd_mem},
    {0, 0, 0}
};

static void skip_spaces(const char** p) {
    while (**p == ' ') (*p)++;
}

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

static void history_load(char* buf, int* pos, int idx) {
    int old = *pos;
    *pos = 0;
    while (history[idx][*pos]) (*pos)++;
    terminal_putchar('\r');
    terminal_write("> ");
    terminal_write(history[idx]);
    for (int i = *pos; i < old; i++) terminal_putchar(' ');
    terminal_putchar('\r');
    terminal_write("> ");
    terminal_write(history[idx]);
    *pos = strlen(history[idx]);
    int j;
    for (j = 0; history[idx][j]; j++) buf[j] = history[idx][j];
    buf[j] = '\0';
}

static void history_clear(char* buf, int* pos) {
    int old = *pos;
    terminal_putchar('\r');
    terminal_write("> ");
    for (int i = 0; i < old; i++) terminal_putchar(' ');
    terminal_putchar('\r');
    terminal_write("> ");
    *pos = 0;
    buf[0] = '\0';
}

static void cmd_help(const char* args) {
    (void)args;
    terminal_write("Available commands:\n");
    for (const struct cmd* c = commands; c->name; c++) {
        terminal_write("  ");
        terminal_write(c->name);
        terminal_write(" - ");
        terminal_write(c->desc);
        terminal_write("\n");
    }
}

static void cmd_clear(const char* args) {
    (void)args;
    terminal_clear();
}

static void cmd_echo(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    terminal_write(p);
    terminal_write("\n");
}

static void cmd_uptime(const char* args) {
    (void)args;
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

static void cmd_version(const char* args) {
    (void)args;
    terminal_write("IridiumOS 32-bit Phase 1\n");
    terminal_write("Boot protocol: Multiboot + PVH ELF note\n");
    terminal_write("CPU: i686 (32-bit protected mode)\n");
}

static void cmd_uname(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    int all = (*p == '-' && p[1] == 'a');

    if (all) {
        terminal_write("IridiumOS iridium 0.1 Phase1 i686\n");
    } else {
        terminal_write("IridiumOS\n");
    }
}

static void cmd_whoami(const char* args) {
    (void)args;
    terminal_write("root\n");
}

static void cmd_id(const char* args) {
    (void)args;
    terminal_write("uid=0(root) gid=0(root)\n");
}

static void cmd_hostname(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    if (*p) {
        int i;
        for (i = 0; *p && i < 63; i++) hostname[i] = *p++;
        hostname[i] = '\0';
    } else {
        terminal_write(hostname);
        terminal_write("\n");
    }
}

static void cmd_true(const char* args) {
    (void)args;
}

static void cmd_false(const char* args) {
    (void)args;
}

static void cmd_mem(const char* args) {
    (void)args;
    uint32_t total = pmm_get_used_frames() + pmm_get_free_frames();
    uint32_t free = pmm_get_free_frames();
    uint32_t used = pmm_get_used_frames();

    terminal_write("Total: ");
    char buf[16];
    int len = 0;
    uint32_t n = total;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) terminal_putchar(buf[i]);
    terminal_write(" frames (");

    // Total MiB
    uint32_t total_mib = (total * 4096) / (1024 * 1024);
    uint32_t used_mib = (used * 4096) / (1024 * 1024);
    uint32_t free_mib = (free * 4096) / (1024 * 1024);

    // Print total MiB
    n = total_mib;
    len = 0;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) terminal_putchar(buf[i]);
    terminal_write(" MiB total, ");

    // Used MiB
    len = 0; n = used_mib;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) terminal_putchar(buf[i]);
    terminal_write(" MiB used, ");

    // Free MiB
    len = 0; n = free_mib;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) terminal_putchar(buf[i]);
    terminal_write(" MiB free)\n");
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
                const char* line = buf;
                skip_spaces(&line);
                const char* start = line;
                while (*line && *line != ' ') line++;
                int cmd_len = line - start;

                int found = 0;
                for (const struct cmd* cmd = commands; cmd->name; cmd++) {
                    if (strncmp(start, cmd->name, cmd_len) == 0 && cmd->name[cmd_len] == '\0') {
                        cmd->handler(buf);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
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
                history_load(buf, &pos, idx);
            }
        } else if (c == KEY_DOWN) {
            if (history_pos > 0) {
                history_pos--;
                int idx = history_count - 1 - history_pos;
                history_load(buf, &pos, idx);
            } else if (history_pos == 0) {
                history_pos = -1;
                history_clear(buf, &pos);
            }
        } else if (c >= 32 && c < 127) {
            if (pos < SHELL_BUF - 1) {
                buf[pos++] = c;
                terminal_putchar(c);
            }
        }
    }
}
