#include "kernel.h"
#include "io.h"
#include "terminal.h"
#include "keyboard.h"
#include "pit.h"

static const char* const exception_names[] = {
    "Divide by zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 FPU error",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Control protection exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security exception",
    "Reserved"
};

struct registers {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, user_esp, user_ss;
};

void isr_handler(struct registers* regs) {
    terminal_write("\nException: ");
    terminal_write(exception_names[regs->int_no]);
    terminal_write("\n");
    __asm__("cli; hlt");
}

void irq_handler(struct registers* regs) {
    if (regs->int_no == 32) {
        pit_tick();
    } else if (regs->int_no == 33) {
        keyboard_handler();
    }

    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
