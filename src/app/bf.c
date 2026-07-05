__asm__(".code16gcc\n");
#include "apps.h"
#include "fs.h"

#define TAPE_SIZE 2048

static unsigned char tape[TAPE_SIZE];
static int ptr;
static const char* prog;
static int pc;

static int match_fwd(void) {
    int depth = 1;
    int p = pc + 1;
    while (prog[p] && depth > 0) {
        if (prog[p] == '[') depth++;
        else if (prog[p] == ']') depth--;
        p++;
    }
    return p - 1;
}

static int match_bwd(void) {
    int depth = 1;
    int p = pc - 1;
    while (p >= 0 && depth > 0) {
        if (prog[p] == ']') depth++;
        else if (prog[p] == '[') depth--;
        p--;
    }
    return p + 1;
}

static void bf_interpret(const char* program) {
    for (int i = 0; i < TAPE_SIZE; i++) tape[i] = 0;
    ptr = TAPE_SIZE / 2;
    pc = 0;
    prog = program;

    while (prog[pc]) {
        char c = prog[pc];
        if (c == '>' && ptr < TAPE_SIZE - 1) ptr++;
        else if (c == '<' && ptr > 0) ptr--;
        else if (c == '+') tape[ptr]++;
        else if (c == '-') tape[ptr]--;
        else if (c == '.') print_char(tape[ptr]);
        else if (c == ',') {
            uint16_t key = get_key();
            tape[ptr] = (char)(key & 0xFF);
        }
        else if (c == '[' && tape[ptr] == 0) pc = match_fwd();
        else if (c == ']' && tape[ptr] != 0) pc = match_bwd();
        pc++;
    }
}

static uint8_t bf_load_saved(void) {
    char buf[512];
    memset(buf, 0, sizeof(buf));
    if (fs_read_file(BF_FILE, (uint8_t*)buf, sizeof(buf) - 1)) return 0;
    for (int i = 0; i < 512; i++) {
        if (buf[i] == '+' || buf[i] == '-' || buf[i] == '<' || buf[i] == '>' ||
            buf[i] == '[' || buf[i] == ']' || buf[i] == '.' || buf[i] == ',') {
            bf_interpret(buf);
            return 1;
        }
    }
    return 0;
}

static void bf_edit(const char* fname) {
    char buf[512];
    int len = 0;
    memset(buf, 0, sizeof(buf));

    if (fs_read_file(fname, (uint8_t*)buf, sizeof(buf) - 1) == 0) {
        while (len < 511 && buf[len]) len++;
    }

    clear_screen();
    print_str("BFEDIT - Ctrl+S save, Ctrl+Q quit\r\n\r\n");
    print_str(buf);

    while (1) {
        uint16_t key = get_key();
        uint8_t c = key & 0xFF;

        if (c == 19) {
            fs_write_file(fname, (uint8_t*)buf, len);
            print_str("\r\n\r\nSaved.\r\n");
            return;
        }
        if (c == 17) {
            print_str("\r\n\r\nCancelled.\r\n");
            return;
        }
        if (c == 8 && len > 0) {
            len--;
            uint8_t r, col;
            get_cursor_rc(&r, &col);
            if (col > 0) { gotoxy(col - 1, r); print_char(' '); gotoxy(col - 1, r); }
            else if (r > 2) { gotoxy(79, r - 1); print_char(' '); gotoxy(79, r - 1); }
            continue;
        }
        if (c >= 32 && c <= 126 && len < 511) {
            buf[len++] = c;
            buf[len] = '\0';
            print_char((char)c);
        }
    }
}

void cmd_bf(const char* args) {
    if (args[0] == '\0') {
        if (!bf_load_saved()) {
            print_str("BF: No saved program. Use 'bfedit' to create one.\r\n");
        }
        print_str("\r\n");
        return;
    }

    int has_dot = 0;
    for (const char* p = args; *p; p++) {
        if (*p == '.') { has_dot = 1; break; }
    }

    if (has_dot) {
        char buf[512];
        memset(buf, 0, sizeof(buf));
        if (fs_read_file(args, (uint8_t*)buf, sizeof(buf) - 1)) {
            print_str("BF: File not found\r\n");
            return;
        }
        bf_interpret(buf);
    } else {
        bf_interpret(args);
    }
    print_str("\r\n");
}

void cmd_bfedit(const char* args) {
    bf_edit((args && args[0]) ? args : BF_FILE);
}
