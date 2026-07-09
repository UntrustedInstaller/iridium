/* IridiumOS — Linker script for the 32-bit kernel ELF */

ENTRY(_start)

SECTIONS
{
    /* The kernel is linked to run at 3 GB + 1 MB (0xC0100000).
       The bootloader maps it to 1 MB (0x100000) physically,
       and we'll set up paging later to map 0xC0100000 → 0x100000.
       For now, we use identity mapping (1 MB) to keep it simple. */
    . = 0x100000;

    .multiboot : {
        *(.multiboot)
    }

    .note : ALIGN(4) {
        *(.note)
        *(.note.*)
    }

    .text : ALIGN(4K) {
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K) {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K) {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        _bss_end = .;
    }

    _kernel_end = .;
}
