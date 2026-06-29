#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// Standard Kernel HAL types
void print_str(const char* str);
void print_char(char c);
uint16_t get_key(void);
uint16_t get_mem_size(void);
void clear_screen(void);
void render_pal_mtx(void);
void print_pal_block(char c1, char c2, uint8_t color);
void print_int(uint16_t val);
void hexdump(const void* addr, int count);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);

#endif