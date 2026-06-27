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

    mov di, 3           ; 3 attempts to boot, if fail, set the carry flag

.read_loop:
    push di             ; Save the attempt counter
    
; Read Stage 2 from disk
    mov ah, 0x02        ; BIOS read sectors function
    mov al, 17          ; Only read the rest of track 0 (17 sectors)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2
    mov dh, 0           ; Head 0
    ; dl is automatically set to the boot drive number by the BIOS on startup

    ; Set destination buffer to 0x1000:0x0000
    mov bx, 0x1000
    mov es, bx
    mov bx, 0x0000
    int 0x13
    
    jnc .success        ; No carry flag? carry on!
    
    ; If at first you don't succeed, try try again!
    xor ax, ax
    int 0x13

    pop di
    dec di
    jnz .read_loop

    jmp dsk_err         ; Jump if carry flag is set (error)

.success
    ; Jump to Stage 2 / Kernel!
    pop di
    jmp 0x1000:0x0000

dsk_err:
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
.halt:
    cli
    hlt
    jmp .halt

times 510-($-$$) db 0
dw 0xaa55