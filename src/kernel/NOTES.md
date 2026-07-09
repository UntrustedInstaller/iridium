# Kernel

Core kernel — the first C code to run after the bootloader hands off.

## Files

- **entry.asm** — Entry point at `_start`. Sets up a 16 KB stack in BSS, pushes multiboot args, calls `kernel_main()`.
- **main.c** — `kernel_main()`. Initializes VGA text mode, prints boot messages, halts.
- **kernel.h** — Shared types and VGA constants for the kernel.

## Future

- GDT setup (bootloader provides one, but we should own it)
- IDT and interrupt handlers
- PIT timer
- PS/2 keyboard driver (raw scancodes → input queue)
- Serial output for debugging
- Memory detection + simple frame allocator
