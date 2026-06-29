#ifndef APPS_H
#define APPS_H

#include "types.h"


// Modular command system pointers for all programs
typedef void (*cmd_func_t)(const char* args);

struct cli_command { 
    const char* name;
    cmd_func_t function;
    const char* description;
};

// Forward declarations for application entry points
void cmd_help(const char* args);
void cmd_clear(const char* args);
void cmd_mem(const char* args);
void cmd_hexdump(const char* args);
void cmd_echo(const char* args);
void cmd_theme(const char* args);
void cmd_palette(const char* args);
void cmd_reboot(const char* args);

#endif