#!/bin/bash
set -e

echo "=============================================="
echo "               IRIDIUM BUILDER                "
echo "=============================================="

# Ensure clean build workspaces exist
mkdir -p build
mkdir -p build/app

# 1. Assemble Stage 1 (Boot Sector) stays raw flat binary
echo "[*] Assembling Stage 1 (src/boot.asm)..."
nasm -f bin src/boot.asm -o build/boot.bin

# 2. Assemble Stage 2 Assembly Base into an ELF32 object layout
echo "[*] Assembling Stage 2 Assembly Base (src/kernel.asm)..."
nasm -f elf32 src/kernel.asm -o build/kernel_asm.o

# 3. Compile Core Kernel C Logic from src/
echo "[*] Compiling Kernel Core (src/main.c)..."
# -Isrc/app tells GCC to find types.h and apps.h inside src/app/
gcc -m16 -march=i386 -ffreestanding -fno-pic -fno-PIE -fno-stack-protector -nostdlib -Isrc/app -c src/main.c -o build/main_c.o

# 4. Automatically discover and compile all modular files inside src/app/
echo "[*] Compiling application modules from src/app/..."
APP_OBJECTS=""

for c_file in src/app/*.c; do
    base_name=$(basename "$c_file" .c)
    echo "    -> Compiling Module: $base_name.c"
    
    # -Isrc/app allows application C files to find apps.h and types.h in their own folder
    gcc -m16 -march=i386 -ffreestanding -fno-pic -fno-PIE -fno-stack-protector -nostdlib -Isrc/app -c "$c_file" -o "build/app/${base_name}.o"
    APP_OBJECTS="$APP_OBJECTS build/app/${base_name}.o"
done

# 5. Link everything together using parent-directory linker.ld script
echo "[*] Linking Iridium via linker.ld into flat kernel binary..."
ld -m elf_i386 -T linker.ld build/kernel_asm.o build/main_c.o $APP_OBJECTS -o build/kernel.bin

# 6. Combine and Pad into build/iridium.img
echo "[*] Synthesizing final floppy disk image..."
cat build/boot.bin build/kernel.bin > build/iridium.img

# Ensure it fits a standard 1.44MB floppy
truncate -s 1474560 build/iridium.img

# Initialize config sector (LBA 50) with default theme 0 (0x1F)
printf '\x1f' | dd of=build/iridium.img bs=512 seek=50 count=1 conv=notrunc status=none 2>/dev/null
echo "[+] Config sector initialized at LBA 50 with default theme."

echo "[+] Build complete: build/iridium.img created successfully!"
echo "----------------------------------------------"

# 7. QEMU test 
read -p "Would you like to test IridiumOS in QEMU right now? (y/N): " run_qemu
if [[ "$run_qemu" =~ ^[Yy]$ ]]; then
    echo "[*] Launching QEMU..."
    qemu-system-i386 -fda build/iridium.img
fi

echo "----------------------------------------------"

# 8. Floppy Disk Burner
read -p "Would you like to write this build to a physical floppy disk? (y/N): " write_floppy
if [[ "$write_floppy" =~ ^[Yy]$ ]]; then
    echo ""
    echo "Current connected block devices:"
    lsblk -d -o NAME,SIZE,TYPE,MODEL
    echo ""
    
    read -p "Enter the target drive path carefully (e.g., /dev/sdX or /dev/fd0): " floppy_dev
    
    if [ -b "$floppy_dev" ]; then
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "WARNING: Writing to $floppy_dev will completely destroy"
        echo "all underlying data on that target drive."
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        read -p "Are you absolutely, 100% sure you want to proceed? (y/N): " confirm
        
        if [[ "$confirm" =~ ^[Yy]$ ]]; then
            echo "[*] Writing to disk..."
            sudo dd if=build/iridium.img of="$floppy_dev" bs=512 status=progress
            sync
            echo "[+] Flash complete!"
        else
            echo "[-] Floppy write aborted safely..."
        fi
    else
        echo "ERR: $floppy_dev is not a valid block device."
    fi
fi