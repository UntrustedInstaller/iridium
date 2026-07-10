#!/bin/sh
set -e

# =========================================================================
#  IridiumOS 32-bit — Build Script
#  POSIX sh compliant.
# =========================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
SRC_DIR="$SCRIPT_DIR/src"

# Terminal colors (ESC sequences for printf)
PURPLE='\033[0;35m'
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

printf "${PURPLE}${BOLD}"
printf "  ╔══════════════════════════════════════════════╗\n"
printf "  ║        IridiumOS 32-bit — Build System       ║\n"
printf "  ║       \"Osmium's periodic neighbor\"           ║\n"
printf "  ╚══════════════════════════════════════════════╝\n"
printf "${NC}"

# ---- Dependency check ----
printf "${BLUE}${BOLD}[*]${NC} Checking build dependencies...\n"
MISSING=""
for cmd in nasm gcc ld objcopy qemu-system-i386; do
    if ! command -v "$cmd" >/dev/null 2>&1; then
        printf "  ${RED}✗${NC} %s ${RED}not found${NC}\n" "$cmd"
        MISSING="$MISSING $cmd"
    else
        printf "  ${GREEN}✓${NC} %s\n" "$cmd"
    fi
done

if [ -n "$MISSING" ]; then
    printf "\n"
    printf "${RED}Missing dependencies:%s${NC}\n" "$MISSING"
    printf "Install them with your package manager, e.g.:\n"
    printf "  sudo apt install nasm gcc binutils qemu-system-x86\n"
    exit 1
fi
printf "\n"

# ---- Clean and prepare build directory ----
printf "${BLUE}${BOLD}[*]${NC} Preparing fresh build directory...\n"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/boot" "$BUILD_DIR/kernel" "$BUILD_DIR/lib"
printf "  ${GREEN}✓${NC} %s\n" "$BUILD_DIR"

# ---- Assemble ----
printf "\n${BLUE}${BOLD}[*]${NC} Assembling (NASM)...\n"
for asm_file in "$SRC_DIR/boot"/*.asm "$SRC_DIR/kernel"/*.asm; do
    [ -f "$asm_file" ] || continue
    base=$(basename "$asm_file" .asm)
    printf "  ${CYAN}→${NC} %s.o\n" "$base"
    nasm -f elf32 "$asm_file" -o "$BUILD_DIR/$base.o"
done

# ---- Compile C ----
printf "\n${BLUE}${BOLD}[*]${NC} Compiling (GCC)...\n"

CFLAGS="-m32 -ffreestanding -fno-stack-protector -fno-pic -fno-PIE"
CFLAGS="$CFLAGS -std=c99 -Wall -Wextra -Werror"
CFLAGS="$CFLAGS -mno-sse -mno-sse2 -mno-mmx -mno-avx"
CFLAGS="$CFLAGS -I$SRC_DIR/kernel -I$SRC_DIR/lib -I$SRC_DIR/drivers -I$SRC_DIR/fs -I$SRC_DIR/gui"
CFLAGS="$CFLAGS -O2 -g"

for c_file in "$SRC_DIR/kernel"/*.c "$SRC_DIR/drivers"/*.c; do
    [ -f "$c_file" ] || continue
    base=$(basename "$c_file" .c)
    printf "  ${CYAN}→${NC} %s.o\n" "$base"
    gcc $CFLAGS -c "$c_file" -o "$BUILD_DIR/$base.o"
done

# ---- Link ----
printf "\n${BLUE}${BOLD}[*]${NC} Linking (LD)...\n"
LDFLAGS="-m elf_i386 -T $SCRIPT_DIR/link.ld"

OBJS=$(find "$BUILD_DIR" -name '*.o' | sort)
# shellcheck disable=SC2086
ld $LDFLAGS $OBJS -o "$BUILD_DIR/iridium.elf"

# ---- Metadata ----
printf "\n${BLUE}${BOLD}[*]${NC} Kernel metadata:\n"
if [ -f "$BUILD_DIR/iridium.elf" ]; then
    KERNEL_SIZE=$(stat -c%s "$BUILD_DIR/iridium.elf")
    printf "  ${GREEN}✓${NC} iridium.elf — ${BOLD}%s bytes${NC} (%s KB)\n" \
        "$KERNEL_SIZE" $((KERNEL_SIZE / 1024))
fi

printf "\n"
text_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.text/ {print $3}' | tr -d '0')
rodata_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.rodata/ {print $3}' | tr -d '0')
bss_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.bss/ {print $3}' | tr -d '0')
printf "  Section        Size\n"
printf "  ─────────────  ─────\n"
printf "  .text          %d\n" "0x$text_size"
printf "  .rodata        %d\n" "0x$rodata_size"
printf "  .bss           %d\n" "0x$bss_size"

# ---- Offer to run ----
printf "\n"
printf "${PURPLE}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
printf "${GREEN}${BOLD}  Build complete!${NC}\n"
printf "${PURPLE}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
printf "\n"

printf "${YELLOW}Run kernel in QEMU?${NC}\n"
printf "  ${CYAN}1)${NC} GTK display (framebuffer)\n"
printf "  ${CYAN}2)${NC} Clean build only\n"

while :; do
    read -r _choice
    case $_choice in
        1)
            printf "\n${BLUE}[*]${NC} Starting QEMU (GTK)...\n"
            qemu-system-i386 \
                -kernel "$BUILD_DIR/iridium.elf" \
                -m 64 \
                -vga vmware \
                -serial stdio \
                -display gtk \
                -no-reboot \
                -no-shutdown
            break
            ;;
        2)
            printf "\n${GREEN}Done.${NC}\n"
            break
            ;;
        *)
            printf "Invalid choice.\n"
            ;;
    esac
done
