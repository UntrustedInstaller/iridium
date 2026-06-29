__asm__(".code16gcc\n");
#include "apps.h"

extern uint8_t cur_col;

void cmd_theme(const char* args) {
    // args will point exactly to the character after "theme "
    if (!args || args[0] == '\0') {
        print_str("ERR: Select theme 0-4\r\n");
        return;
    }

    char choice = args[0];
    if (choice == '0')      cur_col = 0x1F; // White on Blue
    else if (choice == '1') cur_col = 0x02; // Green on Black
    else if (choice == '2') cur_col = 0x06; // Amber on Black
    else if (choice == '3') cur_col = 0x04; // Red on Black
    else if (choice == '4') cur_col = 0x0F; // White on Black
    else {
        print_str("ERR: Select theme 0-4\r\n");
        return;
    }
    
    clear_screen();
    print_str("IridiumOS -- Osmium's periodic neighbor.\r\n");
    print_int(get_mem_size());
    print_str("KB RAM available.\r\n");
    render_pal_mtx();
    print_str("\r\n");
}

void cmd_palette(const char* args) {
    render_pal_mtx();
    print_str("\r\n");
}