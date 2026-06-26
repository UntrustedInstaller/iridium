[org 0x7c00]
bits 16

; =====================================================================
;  BIOS PARAMETER BLOCK
; =====================================================================
jmp short boot_start
nop

db "OSMIUM "
dw 512
db 1
dw 1
db 2
dw 224
dw 2880
db 0xF0
dw 9
dw 18
dw 2
dd 0
dd 0

db 0
db 0
db 0x29
dd 0x12345678
db "OSMIUMOS   "
db "FAT12   "

boot_start:
    xor ax, ax          ; Clear AX
    mov ds, ax          ; Set Data Segment to 0
    mov es, ax          ; Set Extra Segment to 0
    mov ss, ax          ; Set Stack Segment to 0
    mov sp, 0x7c00      ; Set Stack Pointer safely below bootloader

    ; Reset disk drive
    mov ah, 0x00
    int 0x13

; Read Stage 2 from disk
    mov ah, 0x02        ; BIOS read sectors function
    mov al, 32          ; Read 32 sectors safely encompassing string layouts
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2
    mov dh, 0           ; Head 0
    ; dl is automatically set to the boot drive number by the BIOS on startup

    ; Set destination buffer to 0x1000:0x0000
    mov bx, 0x1000
    mov es, bx
    mov bx, 0x0000

    int 0x13
    jc dsk_err       ; Jump if carry flag is set (error)

    ; Jump to Stage 2 / Kernel!
    jmp 0x1000:0x0000

dsk_err:
    ; (Hang or print error message here)
    jmp $

times 510-($-$$) db 0
dw 0xaa55