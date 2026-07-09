#include "keyboard.h"
#include "io.h"
#include "kernel.h"

#define KEYBOARD_DATA 0x60
#define KEYBOARD_CMD  0x64

#define BUFFER_SIZE 256

static volatile char key_buffer[BUFFER_SIZE];
static volatile int buffer_head = 0;
static volatile int buffer_tail = 0;

static const char scancode_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0
};

static const char scancode_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,
    ' ', 0
};

static volatile int shift_pressed = 0;

void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

    if (scancode & 0x80) return;

    char c;
    if (shift_pressed && scancode < sizeof(scancode_shift)) {
        c = scancode_shift[scancode];
    } else if (scancode < sizeof(scancode_ascii)) {
        c = scancode_ascii[scancode];
    } else {
        return;
    }

    if (c) {
        int next = (buffer_head + 1) % BUFFER_SIZE;
        if (next != buffer_tail) {
            key_buffer[buffer_head] = c;
            buffer_head = next;
        }
    }
}

char keyboard_getchar(void) {
    while (buffer_head == buffer_tail);
    char c = key_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    return c;
}

void keyboard_init(void) {
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = 0;
}
