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

_start:
    ; Set up a stack
    mov esp, stack_top

    ; Push multiboot info address (EBX) and magic (EAX) as arguments to kernel_main
    push ebx
    push eax
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
