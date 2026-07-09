#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_UP    0xE0
#define KEY_DOWN  0xE1
#define KEY_LEFT  0xE2
#define KEY_RIGHT 0xE3

void keyboard_init(void);
void keyboard_handler(void);
int  keyboard_getchar(void);

#endif
