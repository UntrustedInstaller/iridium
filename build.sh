#!/bin/bash
set -e

echo "=============================================="
echo "               IRIDIUM BUILDER                "
echo "=============================================="

# Ensure the build directory exists
mkdir -p build

# 1. Assemble Stage 1 (Boot Sector) stays raw flat binary
echo "[*] Assembling Stage 1 (src/boot.asm)..."
nasm -f bin src/boot.asm -o build/boot.bin

# 2. Assemble Stage 2 Assembly Base into an ELF32 object layout
echo "[*] Assembling Stage 2 Assembly Base (src/kernel.asm)..."
nasm -f elf32 src/kernel.asm -o build/kernel_asm.o

# 3. Compile Stage 2 C Components
echo "[*] Compiling Stage 2 C Components (src/main.c)..."
# Added explicit stripping flags to eliminate _GLOBAL_OFFSET_TABLE_ and stack unwind junk

gcc -m16 -march=i386 -ffreestanding -fno-pic -fno-PIE -fno-stack-protector -nostdlib -c src/main.c -o build/main_c.o
# 4. Link everything together without needing an external linker.ld file
echo "[*] Linking Iridium Alloy into flat kernel binary..."
# Forced origin positioning at 0x0 inside the 0x1000 segment execution scope
ld -m elf_i386 -Ttext 0x0 --oformat binary build/kernel_asm.o build/main_c.o -o build/kernel.bin

# 5. Combine and Pad into build/iridium.img
echo "[*] Synthesizing floppy disk image (build/iridium.img)..."
cat build/boot.bin build/kernel.bin > build/combined.tmp

# Create a blank 1.44MB template in the build folder
dd if=/dev/zero of=build/iridium.img bs=1024 count=1440 status=none
# Inject our code into the template
dd if=build/combined.tmp of=build/iridium.img conv=notrunc status=none
truncate -s 1440k build/iridium.img

# Clean up intermediate files
rm build/combined.tmp

echo "[+] Success! Compiled image generated as 'build/iridium.img'"
echo "----------------------------------------------"

# 6. Interactive QEMU Emulation Selection
read -p "Would you like to test Iridium OS in QEMU right now? (y/N): " run_qemu
if [[ "$run_qemu" =~ ^[Yy]$ ]]; then
    echo "[*] Launching QEMU..."
    qemu-system-i386 -fda build/iridium.img
fi

echo "----------------------------------------------"

# 7. Interactive Physical Floppy Disk Burner
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
            echo "[-] Floppy write aborted safely."
        fi
    else
        echo "[-] Error: '$floppy_dev' is not a valid block device. Aborting."
    fi
fi

echo "----------------------------------------------"