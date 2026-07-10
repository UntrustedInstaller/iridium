; Minimal test kernel - just output a character and hang
bits 32
section .text

_start:
    ; Set up stack
    mov esp, 0x100000
    
    ; Initialize serial port (COM1: 0x3F8, 115200 8N1)
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
    
    ; Output test pattern
    mov dx, 0x3F8
    mov al, 'T'
    out dx, al
    mov al, 'E'
    out dx, al
    mov al, 'S'
    out dx, al
    mov al, 'T'
    out dx, al
    mov al, 0x0A
    out dx, al
    
    ; Hang
    cli
    hlt
    jmp $

; === Multiboot header ===
section .multiboot
align 4

MB_MAGIC    equ 0x1BADB002
MB_FLAGS    equ 0x00000043
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

dd MB_MAGIC
dd MB_FLAGS
dd MB_CHECKSUM

; === PVH ELF note (required by QEMU 8+ for -kernel) ===
section .note.Xen
align 4

PVH_NOTE_NAME   equ 4          ; "Xen\0" (4 bytes including null)
PVH_NOTE_DESC   equ 8          ; magic + entry (8 bytes)
PVH_NOTE_TYPE   equ 0x0003     ; XEN_ELFNOTE_HVM_START
PVH_MAGIC       equ 0x33697878 ; "x86h" as uint32

dd PVH_NOTE_NAME
dd PVH_NOTE_DESC
dd PVH_NOTE_TYPE
dd "Xen"                       ; note name (4 bytes, already aligned)
dd 0x33697878                  ; magic "x86h" as uint32
extern _start
dd _start