# IridiumOS Architecture

## Boot flow

1. **BIOS** loads bootloader (GRUB / QEMU `-kernel` multiboot loader)
2. **Bootloader** parses multiboot header in `src/boot/multiboot_header.asm`
3. **Bootloader** loads ELF sections into memory (at `0x100000` physically)
4. **Bootloader** enables A20 gate, enters 32-bit protected mode, sets up flat segments
5. **Bootloader** jumps to `_start` in `src/kernel/entry.asm`
6. **`_start`** sets up a 16 KB stack and calls `kernel_main()`
7. **`kernel_main()`** initializes VGA text mode, prints boot messages, halts

## Memory map (early boot)

| Range | Use |
|-------|-----|
| `0x00000000` – `0x00000400` | IVT (real-mode artifacts, unused in PMode) |
| `0x00010000` – `0x0009FFFF` | Free (conventional memory) |
| `0x000A0000` – `0x000BFFFF` | VGA video memory / framebuffer |
| `0x000B8000` – `0x000B8FA0` | VGA text mode buffer (80×25×2 = 4000 bytes) |
| `0x00100000` + | Kernel (loaded by multiboot) |

## Segment model

- **Flat model**: all segments base `0x00000000`, limit `0xFFFFFFFF`
- Code segment: ring 0, 32-bit, readable + executable
- Data segment: ring 0, 32-bit, read + write
- TSS: eventually for user-mode switching

## Directory reference

| Directory | Contents |
|-----------|----------|
| `src/boot/` | Multiboot header, early boot setup. Only assembly lives here. |
| `src/kernel/` | Core kernel: entry point, main init, HAL primitives. Grows as drivers are added. |
| `src/lib/` | Freestanding runtime: `memset`, `memcpy`, `printf`, `sprintf`, string utils. |
| `src/drivers/` | Hardware drivers: VGA (framebuffer), PS/2 keyboard, IDE/PATA, etc. |
| `src/fs/` | Filesystem support: FAT32, VFS layer (future). |
| `src/gui/` | IMGUI toolkit: compositor, widgets, font renderer, window manager. |
