; IridiumOS — 32-bit protected mode entry point
; Called by the bootloader (multiboot) with a known CPU state.
;
; Multiboot info pointer is in EBX.
; CS is a 32-bit flat code segment. DS/ES/FS/GS/SS are set up.
; A20 gate is already enabled.
; We just need to set up our own stack and jump to C code.

bits 32
section .text

global _start
extern kernel_main
extern multiboot_init

_start:
    ; Set up a stack
    mov esp, stack_top

    ; Save multiboot magic (EAX) and info pointer (EBX) FIRST
    push ebx
    push eax

    ; Early serial init (COM1: 0x3F8, 115200 8N1)
    mov dx, 0x3F8 + 1
    mov al, 0x00
    out dx, al          ; Disable interrupts
    mov dx, 0x3F8 + 3
    mov al, 0x80
    out dx, al          ; DLAB = 1
    mov dx, 0x3F8 + 0
    mov al, 0x0C
    out dx, al          ; Divisor low byte (115200 baud)
    mov dx, 0x3F8 + 1
    mov al, 0x00
    out dx, al          ; Divisor high byte
    mov dx, 0x3F8 + 3
    mov al, 0x03
    out dx, al          ; 8 bits, no parity, 1 stop bit
    mov dx, 0x3F8 + 2
    mov al, 0xC7
    out dx, al          ; Enable FIFO, clear, 14-byte threshold
    mov dx, 0x3F8 + 4
    mov al, 0x0B
    out dx, al          ; IRQs enabled, RTS/DSR set

    ; Call multiboot_init with saved values (already on stack)
    call multiboot_init
    add esp, 8

    call kernel_main

    ; If kernel_main returns, loop forever
halt_loop:
    cli
    hlt
    jmp halt_loop

; void gdt_flush(uint32_t gdt_ptr)
; Loads a new GDT and reloads segment registers.
global gdt_flush
gdt_flush:
    mov eax, [esp + 4]      ; pointer to gdt_ptr struct
    lgdt [eax]

    ; Far jump to reload CS
    jmp 0x08:.reload_cs
.reload_cs:
    ; Reload data segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; void idt_load(uint32_t idt_ptr)
global idt_load
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

section .bss
align 16
stack_bottom:
    resb 16384         ; 16 KB stack — generous for early boot
stack_top: