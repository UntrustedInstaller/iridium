/* src/main.c */
__asm__(".code16gcc\n");

#include "app/types.h"
#include "app/apps.h"

uint8_t cur_col = 0x1F;


// =====================================================================
//  COMMAND TABLE
// =====================================================================
static const struct cli_command cmd_table[] = {
    {"help",    cmd_help,    "Show a list of system commands"},
    {"clear",   cmd_clear,   "Clear the terminal interface"},
    {"mem",     cmd_mem,     "Check how much RAM is available"},
    {"hexdump", cmd_hexdump, "Dump 128 bytes of system memory"},
    {"echo",    cmd_echo,    "Repeat user input to terminal"},
    {"theme",   cmd_theme,   "Quick change color scheme (0-4)"},
    {"palette", cmd_palette, "Render the 16-color palette"},
    {"reboot",  cmd_reboot,  "Soft reboot the machine"}
};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(struct cli_command))

// Dynamic documentation viewer!
void cmd_help(const char* args) {
    print_str("AVAILABLE COMMANDS:\r\n");
    for (int i = 0; i < CMD_COUNT; i++) {
        print_str("  ");
        print_str(cmd_table[i].name);
        
        // Dynamic tpadding
        int len = 0;
        while(cmd_table[i].name[len]) len++;
        for(int s = 0; s < (10 - len); s++) print_char(' ');
        
        print_str(" - ");
        print_str(cmd_table[i].description);
        print_str("\r\n");
    }
}

// =====================================================================
//  HARDWARE ABSTRACTION LAYER 
// =====================================================================
void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void print_str(const char* str) {
    __asm__ __volatile__ ("call asm_print_str" : : "S"(str) : "eax");
}

void print_char(char c) {
    __asm__ __volatile__ ("call asm_print_char" : : "a"(c) : "ebx");
}

void print_pal_block(char c1, char c2, uint8_t color) {
    register uint32_t char1 __asm__("eax") = c1;
    register uint32_t char2 __asm__("ecx") = c2;
    register uint32_t attrib __asm__("ebx") = color;

    __asm__ __volatile__ (
        "pushw %%cx\n\t"
        "movb $0x09, %%ah\n\t" "movb $0, %%bh\n\t" "movb %%bl, %%bl\n\t" "movw $1, %%cx\n\t" "int $0x10\n\t"
        "movb $0x03, %%ah\n\t" "int $0x10\n\t" "incb %%dl\n\t" "movb $0x02, %%ah\n\t" "int $0x10\n\t"
        "popw %%cx\n\t"          
        "movb $0x09, %%ah\n\t" "movb %%cl, %%al\n\t" "pushw %%cx\n\t" "movw $1, %%cx\n\t" "int $0x10\n\t"
        "movb $0x03, %%ah\n\t" "int $0x10\n\t" "incb %%dl\n\t" "movb $0x02, %%ah\n\t" "int $0x10\n\t"
        "popw %%cx\n\t"          
        : : "r"(char1), "r"(char2), "r"(attrib) : "edx", "memory"
    );
}

uint16_t get_key(void) {
    uint16_t key;
    __asm__ __volatile__("movb $0x00, %%ah\n\tint $0x16" : "=a"(key));
    return key;
}

uint16_t get_mem_size(void) {
    uint16_t mem;
    __asm__ __volatile__ ("int $0x12" : "=a"(mem));
    return mem;
}

void clear_screen(void) {
    __asm__ __volatile__ ("call cls" : : : "eax", "ebx", "ecx", "edx");
}

// =====================================================================
//  UTILITIES & RENDERING
// =====================================================================
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void print_int(uint16_t val) {
    char buf[6]; int i = 5; buf[i] = '\0';
    if (val == 0) { print_char('0'); return; }
    while (val > 0) { buf[--i] = (val % 10) + '0'; val /= 10; }
    print_str(&buf[i]);
}

void render_pal_mtx(void) {
    static const char hex_chars[] = "0123456789ABCDEF";
    uint8_t old_theme_col = cur_col;
    for (int i = 0; i < 16; i++) {
        uint8_t attrib = (i << 4) | 0x0F; 
        cur_col = attrib; 
        print_pal_block(hex_chars[i], hex_chars[i], attrib);
    }
    print_pal_block(' ', ' ', old_theme_col);
    cur_col = old_theme_col;
}

void print_hex_byte(uint8_t byte) {
    const char hex_digits[] = "0123456789ABCDEF";
    print_char(hex_digits[(byte>>4) & 0x0F]);
    print_char(hex_digits[byte & 0x0F]);
}

void print_hex_word(uint16_t word) {
    print_hex_byte((word >> 8) & 0xFF);
    print_hex_byte(word & 0xFF);
}

void hexdump(const void* addr, int count) {
    const uint8_t* ptr = (const uint8_t*)addr;
    for (int i = 0; i < count; i += 16) {
        print_hex_word((uint16_t)(uint32_t)(ptr + i));
        print_str(": ");
        for (int j = 0; j < 16; j++) {
            if (i + j < count) { print_hex_byte(ptr[i + j]); print_char(' '); }
            else print_str("   ");
        }
        print_str(" | ");
        for (int j = 0; j < 16; j++) {
            if (i + j < count) {
                uint8_t ch = ptr[i + j];
                if (ch >= 32 && ch <= 126) print_char((char)ch);
                else print_char('.'); 
            } else print_char(' ');
        }
        print_str("|\r\n");
    } 
}

// =====================================================================
//  IRIDIUM SHELL
// =====================================================================
void iridium_main() {
    clear_screen();
    print_str("IridiumOS -- Osmium's periodic neighbor.\r\n");
    print_int(get_mem_size());
    print_str("KB RAM available.\r\n");
    render_pal_mtx();
    print_str("\r\n");

    char cmd_buf[64];
    int cmd_idx = 0;

    while (1) {
        print_str("OS:>");
        cmd_idx = 0;
        for (int i = 0; i < 64; i++) cmd_buf[i] = '\0';

        while (1) {
            uint16_t key = get_key();
            uint8_t ascii = key & 0xFF;

            if (ascii == 13) { 
                print_str("\r\n");
                cmd_buf[cmd_idx] = '\0';
                break;
            }
            else if (ascii == 8) { 
                if (cmd_idx > 0) {
                    cmd_idx--;
                    cmd_buf[cmd_idx] = '\0';
                    print_char(8); print_char(' '); print_char(8);
                }
            }
            else if (ascii >= 32 && ascii <= 126) {
                if (cmd_idx < 63) {
                    cmd_buf[cmd_idx++] = ascii;
                    print_char(ascii);
                }
            }
        }

        if (cmd_idx == 0) continue; 

        // =================================================================
        //  AUTOMATED DISPATCH MECHANISM
        // =================================================================
        int command_executed = 0;

        for (int i = 0; i < CMD_COUNT; i++) {
            int name_len = 0;
            while (cmd_table[i].name[name_len]) name_len++;

            // Precise string checking for clean parsing execution
            if (strcmp(cmd_buf, cmd_table[i].name) == 0) {
                cmd_table[i].function(""); 
                command_executed = 1;
                break;
            } 
            else if (strncmp(cmd_buf, cmd_table[i].name, name_len) == 0 && cmd_buf[name_len] == ' ') {
                cmd_table[i].function(cmd_buf + name_len + 1); 
                command_executed = 1;
                break;
            }
        }

        if (!command_executed) {
            print_str("ERR: Unknown shell command. Type 'help'\r\n");
        }
    }
}