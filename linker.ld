ENTRY(kernel_init)
OUTPUT_FORMAT("binary")

SECTIONS
{
    /* Execute flat at segment offset 0x0000 */
    . = 0x0000;

    .text : {
        *kernel_asm.o(.text)
        *(.text)
        *(.text.*)
    }

    /* Force data sections to follow code immediately with NO page alignment */
    .rodata : SUBALIGN(2) {
        *(.rodata)
        *(.rodata.*)
    }

    .data : SUBALIGN(2) {
        *(.data)
        *(.data.*)
    }

    .bss : SUBALIGN(2) {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}