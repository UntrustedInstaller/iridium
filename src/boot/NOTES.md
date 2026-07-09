# Boot

Contains the multiboot header and any early boot-time assembly.

## Files

- **multiboot_header.asm** — Required multiboot signature. Must be in the first 8192 bytes of the final ELF. Currently declares: align modules (`bit 0`), provide memory map (`bit 1`). No video mode request (VGA text mode default).

## Future

- Replace multiboot with a custom two-stage bootloader (MBR → stage 2 → kernel) for full control
- Support loading from FAT32 on USB/HDD/CD-R
- Floppy variant for Osmium compatibility
- Graphical boot screen at 640×480 VBE
