; IridiumOS — Multiboot header + PVH ELF note
; Required for QEMU -kernel loading (multiboot + PVH).

bits 32

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
; Describes a Xen-compatible HVM kernel entry point.
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
dd PVH_MAGIC
extern _start
dd _start
