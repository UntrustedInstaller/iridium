bits 16

extern module_main
extern _bss_start
extern _bss_end

global _start
_start:
    ; Set up segment registers for the module's segment (0x2000)
    ; Kernel calls us via lcall $0x2000, $0x0000 but leaves DS/ES at kernel segment
    mov ax, 0x2000
    mov ds, ax
    mov es, ax

    ; Zero BSS
    mov cx, _bss_end
    sub cx, _bss_start
    cmp cx, 0
    je .no_bss
    xor ax, ax
    mov di, _bss_start
    cld
    rep stosb
.no_bss:
    ; Use the kernel's stack — lcall already pushed the return address there
    ; Call the module's main function
    o32 call module_main     ; 32-bit call to match C's 32-bit ret (-m16 generates 66 C3)
    ; Give the kernel its segment back so it doesn't paint with the wrong brush
    mov ax, 0x1000
    mov ds, ax
    mov es, ax
    o32 retf                 ; 32-bit far return to match kernel's 32-bit lcall (66 9A pushes 4+2=6 bytes)
