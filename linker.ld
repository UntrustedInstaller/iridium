ENTRY(kernel_init)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x0000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}