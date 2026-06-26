/* src/main.c */

// STOP YELLING AT ME GCC 
__asm__(".code16gcc\n");

// Define explicit types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

// Active terminal color attrib from the kernel
extern uint8_t cur_col;

// All of those declarations to shut the compiler up
void print_str(const char* str);
void print_char(char c);
uint16_t get_key(void);
uint16_t get_mem_size(void);
void clear_screen(void);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);
void print_int(uint16_t val);
void iridium_main(void);

// =====================================================================
//  HARDWARE ABSTRACTION LAYER
// =====================================================================

void print_str(const char* str) {
    __asm__ __volatile__ (
        "call asm_print_str" 
        :
        : "S"(str)
        : "eax"
    );
}

void print_char(char c) {
    __asm__ __volatile__ (
        "call asm_print_char" 
        :
        : "a"(c)
        : "ebx"
    );
}

/* Call BIOS to fetch keyboard interrupts */
uint16_t get_key(void) {
    uint16_t key;
    __asm__ __volatile__(
        "movb $0x00, %%ah\n\t"
        "int $0x16"
        : "=a"(key)
    );
    return key;
}

/* Call BIOS to fetch base memory in KB */
uint16_t get_mem_size(void) {
    uint16_t mem;
    __asm__ __volatile__ (
        "int $0x12"
        : "=a"(mem)
    );
    return mem;
}

void clear_screen(void) {
    __asm__ __volatile__ (
        "call cls"
        :
        :
        : "eax", "ebx", "ecx", "edx"
    );
}

// =====================================================================
//  LIBRARY UTILS
// =====================================================================

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/* Replace print_ram from kernel with easy-to-understand base-10 math (damn you assembly) */
void print_int(uint16_t val) {
    char buf[6];
    int i = 5;
    buf[i] = '\0';

    if (val == 0) {
        print_char('0');
        return;
    }

    while (val > 0) {
        buf[--i] = (val % 10) + '0';
        val /= 10;
    }

    print_str(&buf[i]);
}

// =====================================================================
//  IRIDIUM SHELL
// =====================================================================

void iridium_main() {
    clear_screen();
    print_str("==============================================\r\n");
    print_str("                 IRIDIUM C SHELL              \r\n"); //It's not really a kernel anymore, is it?
    print_str("==============================================\r\n\r\n");

    char cmd_buf[64];
    int cmd_idx = 0;

    while (1) {
        print_str("IR:> ");
        cmd_idx = 0;

        // Clear the buffer properly before accepting input
        for (int i = 0; i < 64; i++) {
            cmd_buf[i] = '\0';
        }

        while (1) {
            uint16_t key = get_key();
            uint8_t ascii = key & 0xFF;

            // Oh my god. Input parsing is so much easier!

            // Enter
            if (ascii == 13) {
                print_str("\r\n");
                // The buffer is already null-terminated by the clearing loop,
                // but this explicitly ensures the string ends here.
                cmd_buf[cmd_idx] = '\0';
                break;
            }
            // Backspace
            else if (ascii == 8) {
                if (cmd_idx > 0) {
                    cmd_idx--;
                    cmd_buf[cmd_idx] = '\0'; // Clear the character in the buffer too
                    print_char(8);
                    print_char(' ');
                    print_char(8);
                }
            }
            // Safe-to-print ASCII (No funny business)
            else if (ascii >= 32 && ascii <= 126) {
                if (cmd_idx < 63) {
                    cmd_buf[cmd_idx++] = ascii;
                    print_char(ascii);
                }
            }
        }

        // Ignore empty lines
        if (cmd_idx == 0) continue; 

        for (int i = 0; i < cmd_idx; i++) {
            if (cmd_buf[i] < 32) {
                cmd_buf[i] = '\0';
                break;
            }
        }

        // =================================================================
        //  COMMAND LOGIC
        // =================================================================

        //This is also much more simple
        char cmd_help[] = "help";
        char cmd_clear[] = "clear";
        char cmd_mem[] = "mem";
        char cmd_echo[] = "echo ";
        char cmd_theme[] = "theme ";
        char cmd_reboot[] = "reboot";

        if (strcmp(cmd_buf, cmd_help) == 0) {
            print_str("AVAILABLE COMMANDS:\r\n");
            print_str("  help     - Show a list of system commands\r\n");
            print_str("  clear    - Clear the terminal interface\r\n");
            print_str("  mem      - Check how much RAM is available to the system\r\n");
            print_str("  echo     - Repeat user input to terminal\r\n");
            print_str("  theme    - Quick change color scheme (0-4)\r\n");
            print_str("  reboot   - Soft reboot the machine\r\n");
        }
        else if (strcmp(cmd_buf, cmd_clear) == 0) {
            clear_screen();
        }
        else if (strcmp(cmd_buf, cmd_mem) == 0) {
            print_str("Memory available: ");
            print_int(get_mem_size());
            print_str(" KB RAM.\r\n");
        }
        else if (strncmp(cmd_buf, cmd_echo, 5) == 0) {
            print_str(cmd_buf + 5);
            print_str("\r\n");
        }
        else if (strncmp(cmd_buf, cmd_theme, 6) == 0) {
            char choice = cmd_buf[6];
            if (choice == '0') cur_col = 0x1F;       // White on Blue (default)
            else if (choice == '1') cur_col = 0x02;  // Green on Black
            else if (choice == '2') cur_col = 0x06;  // Amber on Black
            else if (choice == '3') cur_col = 0x04;  // Red on Black
            else if (choice == '4') cur_col = 0x70;  // Black on White
            else {
                print_str("ERR: Select theme 0-4\r\n");
            }
            clear_screen();
        }
        else if (strcmp(cmd_buf, cmd_reboot) == 0) {
            print_str("Rebooting...\r\n");
            __asm__ __volatile__ ("ljmp $0xFFFF, $0x0000");
        }
        else {
            print_str("ERR: Unknown shell command. Type 'help'\r\n");
        }
    }
}