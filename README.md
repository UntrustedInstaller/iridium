# IridiumOS

**Osmium's periodic neighbor.** A 32-bit protected mode operating system in C and assembly. Sibling to [OsmiumOS](https://github.com/untrustedinstaller/osmium) (16-bit real-mode).

- **Design philosophy**: Playful, functional, 90s-inspired. Motif/Win9X greybevel UI with purple accent. Witty boot messages. Compact GUI toolkit.
- **Current stage**: Bootstrap — kernel boots to VGA text mode, prints messages, halts.
- **Architecture**: 32-bit protected mode, flat memory model, multiboot-compliant (boots with `qemu-system-i386 -kernel` or GRUB).

## Quick start

```bash
bash build.sh
```

Select "Yes, boot it" when prompted. You should see the boot messages on a purple background.

## Directory layout

```
iridium/
├── build.sh              Interactive build script
├── link.ld               Kernel linker script
├── README.md             This file
├── docs/
│   ├── BUILD.md          Detailed build guide
│   └── ARCHITECTURE.md   Kernel design notes
├── src/
│   ├── boot/             Bootloader / multiboot
│   ├── kernel/           Kernel core (entry, main, HAL)
│   ├── lib/              Runtime library (string, printf, etc.)
│   ├── drivers/          Hardware drivers (VGA, keyboard, disk, etc.)
│   ├── fs/               Filesystem layer (FAT32, etc.)
│   └── gui/              IMGUI toolkit
└── build/                Cleaned each run (gitignored)
```

## Dependencies

- `nasm` — assembler
- `gcc` + `ld` — C compiler and linker (32-bit support)
- `qemu-system-i386` — emulator
- `objdump` / `objcopy` — binutils (for inspection)

## Related

- OsmiumOS (16-bit predecessor): `https://github.com/untrustedinstaller/osmium`
