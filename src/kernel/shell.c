#include "kernel.h"
#include "shell.h"
#include "fbterm.h"
#include "terminal.h"
#include "pit.h"
#include "keyboard.h"
#include "pmm.h"

extern int use_fb;

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
static void cmd_pwd(const char*);
static void cmd_yes(const char*);
static void cmd_sleep(const char*);
static void cmd_date(const char*);
static void cmd_env(const char*);

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
    {"pwd",     "Print working directory",    cmd_pwd},
    {"yes",     "Output a string repeatedly", cmd_yes},
    {"sleep",   "Suspend execution for interval", cmd_sleep},
    {"date",    "Print system date and time", cmd_date},
    {"env",     "Print environment",          cmd_env},
    {"printenv","Print environment variables", cmd_env},
    {"mem",     "Show memory usage",          cmd_mem},
    {0, 0, 0}
};

static void skip_spaces(const char** p) {
    while (**p == ' ') (*p)++;
}

static void putch(char c) {
    if (use_fb) fbterm_putchar(c);
    else terminal_putchar(c);
}

static void putstr(const char* s) {
    if (use_fb) fbterm_write(s);
    else terminal_write(s);
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
    putch('\r');
    putstr("> ");
    putstr(history[idx]);
    for (int i = *pos; i < old; i++) putch(' ');
    putch('\r');
    putstr("> ");
    putstr(history[idx]);
    *pos = strlen(history[idx]);
    int j;
    for (j = 0; history[idx][j]; j++) buf[j] = history[idx][j];
    buf[j] = '\0';
}

static void history_clear(char* buf, int* pos) {
    int old = *pos;
    putch('\r');
    putstr("> ");
    for (int i = 0; i < old; i++) putch(' ');
    putch('\r');
    putstr("> ");
    *pos = 0;
    buf[0] = '\0';
}

static void cmd_help(const char* args) {
    (void)args;
    putstr("Available commands:\n");
    for (const struct cmd* c = commands; c->name; c++) {
        putstr("  ");
        putstr(c->name);
        putstr(" - ");
        putstr(c->desc);
        putstr("\n");
    }
}

static void cmd_clear(const char* args) {
    (void)args;
    if (use_fb) fbterm_clear();
    else terminal_clear();
}

static void cmd_echo(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    putstr(p);
    putstr("\n");
}

static void cmd_uptime(const char* args) {
    (void)args;
    uint32_t t = pit_get_tick();
    uint32_t secs = t / 100;
    uint32_t mins = secs / 60;
    uint32_t hours = mins / 60;
    secs %= 60;
    mins %= 60;

    putstr("Uptime: ");
    char buf[32];
    int p = 0;

    if (hours > 0) {
        if (hours > 9) { buf[p++] = '0' + hours / 10; hours %= 10; }
        buf[p++] = '0' + hours;
        buf[p++] = 'h';
    }
    if (mins > 0 || hours > 0) {
        if (mins > 9) { buf[p++] = '0' + mins / 10; mins %= 10; }
        buf[p++] = '0' + mins;
        buf[p++] = 'm';
    }
    buf[p++] = '0' + secs / 10;
    buf[p++] = '0' + secs % 10;
    buf[p++] = 's';
    buf[p++] = '\0';
    putstr(buf);
    putstr("\n");
}

static void cmd_version(const char* args) {
    (void)args;
    putstr("IridiumOS 32-bit Phase 1\n");
    putstr("Boot protocol: Multiboot + PVH ELF note\n");
    putstr("CPU: i686 (32-bit protected mode)\n");
}

static void cmd_uname(const char* args) {
    (void)args;
    putstr("IridiumOS iridium 0.1 Phase1 i686\n");
}

static void cmd_whoami(const char* args) {
    (void)args;
    putstr("root\n");
}

static void cmd_id(const char* args) {
    (void)args;
    putstr("uid=0(root) gid=0(root)\n");
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
        putstr(hostname);
        putstr("\n");
    }
}

static void cmd_true(const char* args) {
    (void)args;
}

static void cmd_false(const char* args) {
    (void)args;
}

static void cmd_pwd(const char* args) {
    (void)args;
    putstr("/\n");
}

static void cmd_yes(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    const char* word = *p ? p : "y";
    while (1) {
        putstr(word);
        putstr("\n");
        for (volatile int i = 0; i < 100000; i++);
    }
}

static void cmd_sleep(const char* args) {
    const char* p = args;
    skip_spaces(&p);
    while (*p && *p != ' ') p++;
    skip_spaces(&p);
    uint32_t secs = 0;
    while (*p >= '0' && *p <= '9') {
        secs = secs * 10 + (*p - '0');
        p++;
    }
    if (secs == 0) secs = 1;
    uint32_t start = pit_get_tick();
    while (pit_get_tick() - start < secs * 100) {
        if (!keyboard_data_available()) continue;
        int c = keyboard_getchar();
        if (c == 0x03) return;
    }
}

static void cmd_date(const char* args) {
    (void)args;
    uint32_t t = pit_get_tick();
    uint32_t uptime = t / 100;
    uint32_t days = uptime / 86400;
    uptime %= 86400;
    uint32_t hours = uptime / 3600;
    uint32_t mins = (uptime % 3600) / 60;
    uint32_t secs = uptime % 60;

    putstr("1970-01-01 +");
    days += 1;
    {
        char dbuf[8];
        int di = 0;
        uint32_t d = days;
        if (d == 0) { dbuf[di++] = '0'; }
        while (d > 0) { dbuf[di++] = '0' + d % 10; d /= 10; }
        while (di--) putch(dbuf[di]);
    }
    putstr(" days ");
    if (hours < 10) putch('0');
    putch('0' + hours / 10);
    putch('0' + hours % 10);
    putch(':');
    if (mins < 10) putch('0');
    putch('0' + mins / 10);
    putch('0' + mins % 10);
    putch(':');
    if (secs < 10) putch('0');
    putch('0' + secs / 10);
    putch('0' + secs % 10);
    putstr("\n");
}

static void cmd_env(const char* args) {
    (void)args;
}

static void cmd_mem(const char* args) {
    (void)args;
    uint32_t total = pmm_get_used_frames() + pmm_get_free_frames();
    uint32_t free = pmm_get_free_frames();
    uint32_t used = pmm_get_used_frames();

    putstr("Total: ");
    char buf[16];
    int len = 0;
    uint32_t n = total;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) putch(buf[i]);
    putstr(" frames (");

    // Total MiB
    uint32_t total_mib = (total * 4096) / (1024 * 1024);
    uint32_t used_mib = (used * 4096) / (1024 * 1024);
    uint32_t free_mib = (free * 4096) / (1024 * 1024);

    // Print total MiB
    n = total_mib;
    len = 0;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) putch(buf[i]);
    putstr(" MiB total, ");

    // Used MiB
    len = 0; n = used_mib;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) putch(buf[i]);
    putstr(" MiB used, ");

    // Free MiB
    len = 0; n = free_mib;
    if (n == 0) { buf[len++] = '0'; }
    else { while (n > 0) { buf[len++] = '0' + n % 10; n /= 10; } }
    for (int i = len - 1; i >= 0; i--) putch(buf[i]);
    putstr(" MiB free)\n");
}

void shell_run(void) {
    char buf[SHELL_BUF];
    int pos = 0;
    int history_pos = -1;

    if (use_fb) fbterm_setcolor(0xFFFFFFFF, 0xFF7845A8);
    putstr("> ");

    uint32_t last_blink = 0;

    while (1) {
        uint32_t now = pit_get_tick();
        if (!keyboard_data_available()) {
            if (now - last_blink >= 25) {
                fbterm_update_cursor();
                last_blink = now;
            }
            continue;
        }
        int c = keyboard_getchar();
        last_blink = now;

        if (c == '\n') {
            putstr("\n");
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
                    putstr("Unknown command. Type 'help'.\n");
                }
            }
            history_pos = -1;
            pos = 0;
            putstr("> ");
        } else if (c == '\b' || c == 0x7F) {
            if (pos > 0) {
                pos--;
                putch('\b');
                putch(' ');
                putch('\b');
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
                history_clear(buf, &pos);
                history_pos = -1;
            }
        } else if (c >= 0x20 && c <= 0x7E) {
            if (pos < SHELL_BUF - 1) {
                buf[pos++] = (char)c;
                putch((char)c);
            }
        }
    }
}