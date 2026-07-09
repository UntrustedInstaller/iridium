; IridiumOS — Interrupt Service Routine stubs
; Uses macros to generate 256 unique entry points that push
; the interrupt number and (if needed) a dummy error code,
; then jump to the common handler.

bits 32
section .text

; External common handler written in C
extern isr_handler
extern irq_handler

; Macro for exceptions that DO push an error code
%macro ISR_ERROR 1
global isr%1
isr%1:
    push %1
    jmp isr_common
%endmacro

; Macro for exceptions that do NOT push an error code
%macro ISR_NOERROR 1
global isr%1
isr%1:
    push 0          ; dummy error code
    push %1
    jmp isr_common
%endmacro

; Macro for IRQ handlers
%macro IRQ 2
global irq%1
irq%1:
    push 0
    push %2
    jmp irq_common
%endmacro

isr_common:
    ; Save all registers
    pusha
    push ds
    push es
    push fs
    push gs

    ; Use kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Pass stack frame pointer to C handler
    mov eax, esp
    push eax
    call isr_handler
    add esp, 4

    ; Restore registers
    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; Clean up the pushed error code and interrupt number
    add esp, 8
    iret

irq_common:
    ; Save all registers
    pusha
    push ds
    push es
    push fs
    push gs

    ; Use kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Pass stack frame pointer to C handler
    mov eax, esp
    push eax
    call irq_handler
    add esp, 4

    ; Restore registers
    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; Clean up the pushed error code and interrupt number
    add esp, 8
    iret

; CPU exceptions 0-31
ISR_NOERROR 0     ; Division by zero
ISR_NOERROR 1     ; Debug
ISR_NOERROR 2     ; NMI
ISR_NOERROR 3     ; Breakpoint
ISR_NOERROR 4     ; Overflow
ISR_NOERROR 5     ; Bound range exceeded
ISR_NOERROR 6     ; Invalid opcode
ISR_NOERROR 7     ; Device not available
ISR_ERROR   8     ; Double fault (has error code)
ISR_NOERROR 9     ; Coprocessor segment overrun
ISR_ERROR   10    ; Invalid TSS
ISR_ERROR   11    ; Segment not present
ISR_ERROR   12    ; Stack segment fault
ISR_ERROR   13    ; General protection fault
ISR_ERROR   14    ; Page fault
ISR_NOERROR 15    ; Reserved
ISR_NOERROR 16    ; x87 FPU error
ISR_ERROR   17    ; Alignment check
ISR_NOERROR 18    ; Machine check
ISR_NOERROR 19    ; SIMD floating-point exception
ISR_NOERROR 20    ; Virtualization exception
ISR_ERROR   21    ; Control protection exception
ISR_NOERROR 22    ; Reserved
ISR_NOERROR 23    ; Reserved
ISR_NOERROR 24    ; Reserved
ISR_NOERROR 25    ; Reserved
ISR_NOERROR 26    ; Reserved
ISR_NOERROR 27    ; Reserved
ISR_NOERROR 28    ; Reserved
ISR_NOERROR 29    ; Reserved
ISR_ERROR   30    ; Security exception
ISR_NOERROR 31    ; Reserved

; IRQ handlers 32-47 (mapped from IRQ0-15)
IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
