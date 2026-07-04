__asm__(".code16gcc\n");
#include "apps.h"

static char ed_buf[EDITOR_MAX_SIZE];
static int ed_size;
static int ed_cursor;
static int ed_scroll;
static int ed_modified;

static int line_start(int pos) {
    while (pos > 0 && ed_buf[pos - 1] != '\n') pos--;
    return pos;
}

static int line_end(int pos) {
    while (pos < ed_size && ed_buf[pos] != '\n') pos++;
    return pos;
}

static void ensure_visible(void) {
    for (int limit = 0; limit < 100; limit++) {
        int line = 2;
        int p = ed_scroll;
        while (p < ed_cursor && p < ed_size) {
            if (ed_buf[p] == '\n') line++;
            p++;
        }
        if (line < 2 && ed_scroll > 0) {
            int ns = ed_scroll;
            while (ns > 0 && ed_buf[ns - 1] != '\n') ns--;
            if (ns > 0) ns--;
            ed_scroll = ns;
        } else if (line >= 24) {
            if (ed_scroll >= ed_size) break;
            while (ed_scroll < ed_size && ed_buf[ed_scroll] != '\n') ed_scroll++;
            if (ed_scroll < ed_size) ed_scroll++;
        } else break;
    }
}

static void ed_render(void) {
    ensure_visible();

    for (int r = 2; r < 24; r++) {
        gotoxy(0, r);
        for (int c = 0; c < 80; c++) print_char(' ');
    }

    int p = ed_scroll;
    for (int r = 2; r < 24; r++) {
        gotoxy(0, r);
        int c = 0;
        while (p < ed_size && ed_buf[p] != '\n' && c < 80) {
            print_char(ed_buf[p]); p++; c++;
        }
        while (c < 80) { print_char(' '); c++; }
        if (p < ed_size && ed_buf[p] == '\n') p++;
    }

    gotoxy(0, 0);
    print_str("IRIDIUM TEXT EDITOR");
    if (ed_modified) {
        gotoxy(28, 0);
        print_str("[Modified]");
    }

    int ln = 1, ls = 0;
    for (int i = 0; i < ed_cursor && i < ed_size; i++) {
        if (ed_buf[i] == '\n') { ln++; ls = i + 1; }
    }
    gotoxy(52, 0);
    print_str("Ln: ");
    print_int(ln);
    gotoxy(62, 0);
    print_str("Col: ");
    print_int(ed_cursor - ls + 1);

    gotoxy(0, 24);
    print_str("Ctrl+S=Save  Ctrl+Q=Quit  INS");

    int sy = 2;
    int sp = ed_scroll;
    while (sp < ed_cursor && sp < ed_size) {
        if (ed_buf[sp] == '\n') sy++;
        sp++;
    }
    int sx = ed_cursor - line_start(ed_cursor);
    if (sx > 79) sx = 79;
    if (sy > 23) sy = 23;
    gotoxy(sx, sy);
}

static void ed_insert(char c) {
    if (ed_size >= EDITOR_MAX_SIZE - 1) return;
    for (int i = ed_size; i > ed_cursor; i--) ed_buf[i] = ed_buf[i - 1];
    ed_buf[ed_cursor] = c;
    ed_size++;
    ed_cursor++;
    ed_modified = 1;
}

static void ed_backspace(void) {
    if (ed_cursor <= 0) return;
    for (int i = ed_cursor - 1; i < ed_size; i++) ed_buf[i] = ed_buf[i + 1];
    ed_size--;
    ed_cursor--;
    ed_modified = 1;
}

static void ed_delete(void) {
    if (ed_cursor >= ed_size) return;
    for (int i = ed_cursor; i < ed_size; i++) ed_buf[i] = ed_buf[i + 1];
    ed_size--;
    ed_modified = 1;
}

static void cur_left(void) {
    if (ed_cursor > 0) ed_cursor--;
}

static void cur_right(void) {
    if (ed_cursor < ed_size) ed_cursor++;
}

static void cur_up(void) {
    if (ed_cursor <= 0) { ed_cursor = 0; return; }
    int cs = line_start(ed_cursor);
    if (cs == 0) { ed_cursor = 0; return; }
    int ps = line_start(cs - 1);
    int col = ed_cursor - cs;
    ed_cursor = ps + col;
    int pe = line_end(ps);
    if (ed_cursor > pe) ed_cursor = pe;
}

static void cur_down(void) {
    if (ed_cursor >= ed_size) { ed_cursor = ed_size; return; }
    int ce = line_end(ed_cursor);
    if (ce >= ed_size) { ed_cursor = ed_size; return; }
    int ns = ce + 1;
    int cs = line_start(ed_cursor);
    int col = ed_cursor - cs;
    ed_cursor = ns + col;
    int ne = line_end(ns);
    if (ed_cursor > ne) ed_cursor = ne;
}

static void ed_load(void) {
    ed_size = 0;
    for (int s = 0; s < EDITOR_SECTORS; s++) {
        if (read_sector(EDITOR_SECTOR + s, ed_buf + s * 512)) return;
    }
    while (ed_size < EDITOR_MAX_SIZE && ed_buf[ed_size]) ed_size++;
}

static void ed_save(void) {
    ed_buf[ed_size] = '\0';
    for (int i = ed_size + 1; i < EDITOR_MAX_SIZE; i++) ed_buf[i] = 0;
    for (int s = 0; s < EDITOR_SECTORS; s++) {
        if (write_sector(EDITOR_SECTOR + s, ed_buf + s * 512)) {
            print_str("ERR: Failed to save file\r\n");
            return;
        }
    }
}

void cmd_edit(const char* args) {
    clear_screen();

    for (int i = 0; i < EDITOR_MAX_SIZE; i++) ed_buf[i] = 0;
    ed_size = 0;
    ed_cursor = 0;
    ed_scroll = 0;
    ed_modified = 0;

    ed_load();

    while (1) {
        ed_render();

        uint16_t key = get_key();
        uint8_t ascii = key & 0xFF;
        uint8_t scan = (key >> 8) & 0xFF;

        if (ascii == 19) {
            ed_save();
            ed_modified = 0;
        } else if (ascii == 17 || ascii == 27) {
            break;
        } else if (ascii == 13) {
            ed_insert('\n');
        } else if (ascii == 8) {
            ed_backspace();
        } else if (ascii == 0) {
            if (scan == 0x48) cur_up();
            else if (scan == 0x50) cur_down();
            else if (scan == 0x4B) cur_left();
            else if (scan == 0x4D) cur_right();
            else if (scan == 0x47) ed_cursor = line_start(ed_cursor);
            else if (scan == 0x4F) ed_cursor = line_end(ed_cursor);
            else if (scan == 0x53) ed_delete();
        } else if (ascii >= 32 && ascii <= 126) {
            ed_insert((char)ascii);
        }
    }

    clear_screen();
}
