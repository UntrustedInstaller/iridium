bits 16 ; Do I need to explain this?

; Set all those pesky global calls
global kernel_init
global asm_print_str
global asm_print_char
global cls

; Calling the shell function from the C file
extern iridium_main
extern cur_col

; =====================================================================
;    KERNEL INITIALIZATION
; =====================================================================

kernel_init:
    cld

                        ; Sync data segment registers to 0x1000
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

                        ; Sync Stack Segment to 0x1000
    mov ss, ax
    
                        ; Clear the entire 32-bit ESP/EBP registers to wipe BIOS garbage
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
    push ds
    push es

    mov ax, 0x1000
    mov ds, ax
    mov es, ax

.loop:
    lodsb
    cmp al, 0
    je .done
    ; Route every character through asm_print_char so scroll
    ; logic is applied consistently in one place
    push si
    mov ah, al          ; stash char
    ; call the char routine inline to avoid segment weirdness
    mov al, ah
    call do_print_char
    pop si
    jmp .loop
.done:
    pop es
    pop ds
    popa
    ret

asm_print_char:
    push ax
    push bx
    push cx
    push dx

    mov ah, al              ; preserve the character
    call do_print_char

    pop dx
    pop cx
    pop bx
    pop ax
    ret

do_print_char:
    push ax
    push bx
    push cx
    push dx

    cmp al, 13              ; \r  – just move cursor to column 0
    je .do_cr
    cmp al, 10              ; \n  – advance row, scroll if needed
    je .do_lf
    cmp al, 8               ; BS  – move cursor left one column
    je .do_bs

    ; Normal character
    mov ah, 0x09            ; Write char + attribute at cursor (does NOT move cursor)
    mov bh, 0x00            ; Page 0
    mov bl, [cur_col]       ; Current colour attribute
    mov cx, 1               ; Write exactly 1 copy
    int 0x10

    ; Advance cursor one column manually
    mov ah, 0x03            ; Get cursor position → DH=row, DL=col
    mov bh, 0x00
    int 0x10
    inc dl                  ; Move right one column
    cmp dl, 80              ; Past end of line?
    jl .set_cursor
    mov dl, 0               ; Wrap to column 0 …
    inc dh                  ; … and drop to next row
    cmp dh, 25              ; Past last row (rows 0-24)?
    jl .set_cursor
    call do_scroll          ; Scroll up and stay on row 24
    mov dh, 24
    mov dl, 0
    jmp .set_cursor

    ; Backspace
.do_bs:
    mov ah, 0x03            ; Get current cursor position
    mov bh, 0x00
    int 0x10                ; DH = row, DL = col
    test dl, dl             ; Already at column 0?
    jz .bs_done             ; If so, do nothing (don't walk off the line)
    dec dl                  ; Move left one column
    mov ah, 0x02            ; Set cursor
    mov bh, 0x00
    int 0x10
.bs_done:
    jmp .exit               ; Skip the set_cursor at the bottom (already set)

    ; Carriage Return
.do_cr:
    mov ah, 0x03
    mov bh, 0x00
    int 0x10                ; DH = current row, DL = current col
    mov dl, 0               ; Column 0, same row
    jmp .set_cursor

    ; Line Feed
.do_lf:
    mov ah, 0x03
    mov bh, 0x00
    int 0x10                ; DH = current row
    inc dh
    cmp dh, 25
    jl .set_cursor          ; Still on screen? just move down
    call do_scroll          ; Last row? scroll, keep cursor on row 24
    mov dh, 24
    ; DL already holds the column from INT 10h/03h

.set_cursor:
    mov ah, 0x02
    mov bh, 0x00
    int 0x10

.exit:
    pop dx
    pop cx
    pop bx
    pop ax
    ret

do_scroll:
    push ax
    push bx
    push cx
    push dx

    mov ah, 0x06            ; Scroll up
    mov al, 1               ; Scroll 1 line
    mov bh, [cur_col]       ; Fill attribute  ← THIS is the fix
    mov cx, 0x0000          ; Top-left  (row 0, col 0)
    mov dx, 0x184F          ; Bottom-right (row 24, col 79)
    int 0x10

    pop dx
    pop cx
    pop bx
    pop ax
    ret

cls:
    pusha
    mov ah, 0x06        ; Scroll up window function
    mov al, 0           ; Clear entire screen
    mov bh, [cur_col]   ; Use active color attribute to paint background
    mov cx, 0x0000      ; Top-left corner (Row 0, Col 0)
    mov dx, 0x184F      ; Bottom-right corner (Row 24, Col 79)
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
;    DISK I/O HAL
; =====================================================================

global asm_read_sector
global asm_write_sector

; Read one sector via BIOS int 13h
; Input:  AX = LBA, BX = buffer offset (within DS)
; Output: AX = 0 success, 1 failure
asm_read_sector:
    push cx
    push dx
    push bx

    call lba_to_chs

    pop bx
    push ds
    pop es
    mov ah, 0x02
    mov al, 1
    mov dl, [boot_drive]
    int 0x13

    mov ax, 0
    jnc .read_ok
    mov ax, 1
.read_ok:
    pop dx
    pop cx
    ret

; Write one sector via BIOS int 13h
; Input:  AX = LBA, BX = buffer offset (within DS)
; Output: AX = 0 success, 1 failure
asm_write_sector:
    push cx
    push dx
    push bx

    call lba_to_chs

    pop bx
    push ds
    pop es
    mov ah, 0x03
    mov al, 1
    mov dl, [boot_drive]
    int 0x13

    mov ax, 0
    jnc .write_ok
    mov ax, 1
.write_ok:
    pop dx
    pop cx
    ret

; Convert LBA to CHS for 1.44MB floppy (80 cyl, 2 heads, 18 sect/track)
; Input:  AX = LBA (0-2879)
; Output: CH = cylinder, CL = sector, DH = head
; Preserves: DL (drive number)
lba_to_chs:
    push ax
    push bx
    push dx

    xor dx, dx
    mov bx, 18
    div bx             ; AX = LBA/18, DX = LBA%18

    mov bx, ax         ; BX = LBA/18
    mov cl, dl
    add cl, 1          ; CL = sector (1-18)

    mov ax, bx
    xor dx, dx
    mov bx, 2
    div bx             ; AX = cylinder, DX = head

    mov ch, al         ; CH = cylinder (0-79)
    mov bx, dx         ; BX = head
    pop dx             ; DX = original (DL preserved)
    mov dh, bl         ; DH = head

    pop bx
    pop ax
    ret

; =====================================================================
;    KERNEL DATA & VARIABLE STORAGE
; =====================================================================

global boot_drive
boot_drive  db 0

; Everything after this in the original kernel file gets migrated over to C
; Not only does this make code more readable, it frees me from the 
; assembly portion of the OS, saving my sanity