# Building IridiumOS

## Interactive build

```bash
bash build.sh
```

This will check dependencies, compile everything, link the kernel ELF, and offer to boot it in QEMU.

## Manual build

```bash
export BUILD=build
mkdir -p $BUILD

# Assemble multiboot header
nasm -f elf32 src/boot/multiboot_header.asm -o $BUILD/multiboot_header.o

# Assemble entry point
nasm -f elf32 src/kernel/entry.asm -o $BUILD/entry.o

# Compile C sources
gcc -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-PIE \
    -std=c99 -Wall -Wextra -Werror -O2 -g \
    -Isrc/kernel -c src/kernel/main.c -o $BUILD/main.o

# Link
ld -m elf_i386 -T link.ld \
    $BUILD/multiboot_header.o $BUILD/entry.o $BUILD/main.o \
    -o $BUILD/iridium.elf

# Run
qemu-system-i386 -kernel $BUILD/iridium.elf -m 64
```

## Toolchain notes

The kernel is compiled with `-m32 -ffreestanding` (no libc, no startup code). The multiboot header tells the bootloader (GRUB or QEMU's built-in multiboot loader) how to load the ELF.

On some systems, 32-bit compilation support needs a separate package:

```bash
# Debian/Ubuntu
sudo apt install gcc-multilib

# Arch
sudo pacman -S gcc-multilib

# Fedora
sudo dnf install glibc-devel.i686
```

## Output

The build produces `build/iridium.elf` — a 32-bit ELF kernel with the multiboot header in the first 8 KB. Boot it with any multiboot-compliant loader.
