bits 16 ; Do I need to explain this?

; Set all those pesky global calls
global kernel_init
global asm_print_str
global asm_print_char
global cls
global cur_col

; Calling the shell function from the C file
extern iridium_main

; =====================================================================
;    KERNEL INITIALIZATION
; =====================================================================

kernel_init:
    cld

    ; 1. Sync data segment registers to 0x1000
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    ; 2. Sync Stack Segment to 0x1000
    mov ss, ax
    
    ; 3. Clear the entire 32-bit ESP/EBP registers to wipe BIOS garbage
    xor esp, esp        ; Zero out the full 32-bit register
    mov sp, 0xFFF0      ; Assign SP safely to the top of the 0x1000 segment boundary
    xor ebp, ebp        ; Zero out the full 32-bit EBP register
    mov bp, sp          ; Align base pointer with stack pointer

    mov [boot_drive], dl

    ; Intensive backgrounds and disable blinking (IBM PC leftovers)
    mov ax, 0x1003
    mov bl, 0x00        ; Disable blinking / enable intensive backgrounds
    int 0x10

    ; Initial screen clear and paint
    call cls

    ; BEEP!
    mov ah, 0x0E
    mov al, 7
    int 0x10

    ; Jump to the C shell
    call iridium_main

    ; Safety net catch if C somehow crashes or returns
    cli
.halt:
    hlt
    jmp .halt


; =====================================================================
;    LOW-LEVEL HARDWARE UTILITIES (HAL)
; =====================================================================

asm_print_str:
    pusha
    push ds             ; Save data segment
    push es             ; Save extra segment
    
    mov ax, 0x1000      ; Ensure segments are locked to our flat binary base
    mov ds, ax
    mov es, ax

    mov ah, 0x0E        ; BIOS teletype function
.loop:
    lodsb               ; Load byte from DS:SI into AL, increment SI
    cmp al, 0           ; Check for null terminator
    je .done
    mov bh, 0x00        ; Page number 0
    mov bl, [cur_col]   ; Use the active color attribute
    int 0x10            ; Call BIOS video interrupt
    jmp .loop
.done:
    pop es
    pop ds
    popa
    ret

asm_print_char:
    ; Only push what we absolutely need to preserve
    push ax
    push bx
    
    mov ah, 0x0E        ; BIOS teletype function
    mov bh, 0x00        ; Page number 0
    mov bl, [cur_col]   ; Explicitly set the active color attribute
    int 0x10            ; Call BIOS video service
    
    pop bx
    pop ax
    ret

cls:
    pusha
    mov ah, 0x06        ; Scroll up window function
    mov al, 0           ; Clear entire screen
    mov bh, [cur_col]   ; Use active color attribute to paint background
    mov cx, 0x0000      ; Top-left corner (Row 0, Col 0)
    mov dx, 0x194F      ; Bottom-right corner (Row 25, Col 80)
    int 0x10            ; Call BIOS video interrupt

    ; Reset cursor position back to 0,0
    mov ah, 0x02
    mov bh, 0
    mov dh, 0
    mov dl, 0
    int 0x10
    popa
    ret


; =====================================================================
;    KERNEL DATA & VARIABLE STORAGE
; =====================================================================

boot_drive  db 0
cur_col     db 0x1F     ; White text on Blue background (Default)

; Everything after this in the original kernel file gets migrated over to C
; Not only does this make code more readable, it frees me from the 
; assembly portion of the OS, saving my sanity