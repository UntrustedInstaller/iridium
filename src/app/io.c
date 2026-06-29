__asm__(".code16gcc\n");
#include "apps.h"

void cmd_echo(const char* args) {
    print_str(args);
    print_str("\r\n");
}

void cmd_hexdump(const char* args) {
    print_str("Dumping segment 0x1000:0x0000:\r\n");
    hexdump((const void*)0x0000, 128);
}