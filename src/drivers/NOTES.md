# Drivers

Hardware abstraction layer for IridiumOS.

## Planned

### VGA / Framebuffer
- Text mode driver (80×25, existing in kernel)
- VBE framebuffer driver (640×480 or 1024×768, 32bpp)
- Font renderer (8×16 bitmap glyphs)
- Double-buffered compositor surface

### Input
- PS/2 keyboard driver (scancode set 1 translation)
- PS/2 mouse driver (for GUI)
- Event queue / input system

### Storage
- IDE PATA driver (primary + secondary channels)
- LBA48 support for large disks
- AHCI (SATA) — stretch goal

### Other
- PIT (programmable interval timer) — system tick
- CMOS/RTC — date and time
- PC speaker — beep
- ACPI — shutdown, reboot
