#!/usr/bin/env bash
set -e

# =========================================================================
#  IridiumOS 32-bit — Build Script
#  Interactive, self-documenting, fresh build each run.
# =========================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
SRC_DIR="$SCRIPT_DIR/src"

PURPLE='\033[0;35m'
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${PURPLE}${BOLD}"
echo "  ╔══════════════════════════════════════════════╗"
echo "  ║        IridiumOS 32-bit — Build System       ║"
echo "  ║       \"Osmium's periodic neighbor\"           ║"
echo "  ╚══════════════════════════════════════════════╝"
echo -e "${NC}"

# ---- Dependency check ----
echo -e "${BLUE}${BOLD}[*]${NC} Checking build dependencies..."
MISSING=""
for cmd in nasm gcc ld objcopy qemu-system-i386; do
    if ! command -v $cmd &>/dev/null; then
        echo -e "  ${RED}✗${NC} $cmd ${RED}not found${NC}"
        MISSING="$MISSING $cmd"
    else
        echo -e "  ${GREEN}✓${NC} $cmd"
    fi
done

if [ -n "$MISSING" ]; then
    echo ""
    echo -e "${RED}Missing dependencies:$MISSING${NC}"
    echo "Install them with your package manager, e.g.:"
    echo "  sudo apt install nasm gcc binutils qemu-system-x86"
    exit 1
fi
echo ""

# ---- Clean and prepare build directory ----
echo -e "${BLUE}${BOLD}[*]${NC} Preparing fresh build directory..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"/{boot,kernel,lib}
echo -e "  ${GREEN}✓${NC} $BUILD_DIR"

# ---- Assemble ----
echo -e "\n${BLUE}${BOLD}[*]${NC} Assembling (NASM)..."
for asm_file in "$SRC_DIR"/boot/*.asm "$SRC_DIR"/kernel/*.asm; do
    [ -f "$asm_file" ] || continue
    base=$(basename "$asm_file" .asm)
    echo -e "  ${CYAN}→${NC} $base.o"
    nasm -f elf32 "$asm_file" -o "$BUILD_DIR/$base.o"
done

# ---- Compile C ----
echo -e "\n${BLUE}${BOLD}[*]${NC} Compiling (GCC)..."

CFLAGS="-m32 -ffreestanding -fno-stack-protector -fno-pic -fno-PIE"
CFLAGS="$CFLAGS -std=c99 -Wall -Wextra -Werror"
CFLAGS="$CFLAGS -I$SRC_DIR/kernel -I$SRC_DIR/lib -I$SRC_DIR/drivers -I$SRC_DIR/fs -I$SRC_DIR/gui"
CFLAGS="$CFLAGS -O2 -g"

for c_file in "$SRC_DIR"/kernel/*.c; do
    [ -f "$c_file" ] || continue
    base=$(basename "$c_file" .c)
    echo -e "  ${CYAN}→${NC} $base.o"
    gcc $CFLAGS -c "$c_file" -o "$BUILD_DIR/$base.o"
done

# ---- Link ----
echo -e "\n${BLUE}${BOLD}[*]${NC} Linking (LD)..."
LDFLAGS="-m elf_i386 -T $SCRIPT_DIR/link.ld"

# Collect all object files
OBJS=$(find "$BUILD_DIR" -name '*.o' | sort)
ld $LDFLAGS $OBJS -o "$BUILD_DIR/iridium.elf"

# ---- Metadata ----
echo -e "\n${BLUE}${BOLD}[*]${NC} Kernel metadata:"
KERNEL_SIZE=$(stat -c%s "$BUILD_DIR/iridium.elf")
echo -e "  ${GREEN}✓${NC} iridium.elf — ${BOLD}$KERNEL_SIZE bytes${NC} ($((KERNEL_SIZE / 1024)) KB)"

# Show section sizes
echo ""
text_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.text/ {print $3}' | tr -d '0')
rodata_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.rodata/ {print $3}' | tr -d '0')
bss_size=$(objdump -h "$BUILD_DIR/iridium.elf" 2>/dev/null | awk '/\.bss/ {print $3}' | tr -d '0')
echo "  Section        Size"
echo "  ─────────────  ─────"
printf "  .text          %d\n" 0x$text_size
printf "  .rodata        %d\n" 0x$rodata_size
printf "  .bss           %d\n" 0x$bss_size

# ---- Offer to run ----
echo ""
echo -e "${PURPLE}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}${BOLD}  Build complete!${NC}"
echo -e "${PURPLE}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo -e "${YELLOW}Run kernel in QEMU?${NC}"
select yn in "Yes, boot it" "No, exit"; do
    case $yn in
        "Yes, boot it" )
            echo ""
            echo -e "${BLUE}[*]${NC} Starting QEMU..."
            echo -e "  ${GREEN}→${NC} qemu-system-i386 -kernel $BUILD_DIR/iridium.elf${NC}"
            echo ""
            qemu-system-i386 \
                -kernel "$BUILD_DIR/iridium.elf" \
                -m 64 \
                -serial stdio \
                -no-reboot \
                -no-shutdown
            break
            ;;
        "No, exit" )
            echo ""
            echo -e "${GREEN}Done.${NC}"
            break
            ;;
    esac
done
