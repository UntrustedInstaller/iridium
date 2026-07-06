# Module Development Guide

A "module" is a flat binary loaded at segment `0x2000` and called via `lcall`. No COFF, no ELF, no DOS .COM nonsense — just raw machine code that starts executing at offset 0.

## Lifecycle

1. User types a command in the shell
2. Shell tries the command table first (built-in commands win)
3. If no match, it auto-appends `.BIN` and looks for the file on disk
4. Found → loaded to `0x2000:0x0000`, BSS zeroed, `module_main()` called
5. Not found → "Unknown command"
6. Module returns via `retf` → back to shell

You can also use `exec <name>` to force-run a module by name.

## Template

```c
__asm__(".code16gcc\n");
#include "api.h"        /* INT 60h wrappers */
#include "types.h"      /* uint8_t, etc. */

/* BSS — gets zeroed by module_entry.asm */
static char buf[256];

void module_main(void) {
    /* args are at 0x2000:0xFC00 if any */
    char* args = (char*)0xFC00;

    print_str("Hello from my module!\r\n");
}
```

Compile with:

```sh
gcc -m16 -march=i386 -ffreestanding -nostdlib -Isrc/app \
    -c mymodule.c -o mymodule.o

nasm -f elf32 src/module_entry.asm -o mod_entry.o

ld -m elf_i386 -T src/module.ld mod_entry.o mymodule.o \
    -o mymodule.bin
```

Drop `mymodule.bin` on the floppy and run it.

## API (INT 60h)

| CX | Function | Params | Returns |
|----|----------|--------|---------|
| 0  | print_str | ES:BX = string | — |
| 1  | print_char | AL = char | — |
| 2  | get_key | — | AX = scancode |
| 3  | clear_screen | — | — |
| 4  | gotoxy | DL = col, DH = row | — |
| 5  | read_sector | AX = LBA, ES:BX = buf | AL = 0 ok |
| 6  | write_sector | AX = LBA, ES:BX = buf | AL = 0 ok |
| 7  | get_cursor | — | DL = row, DH = col |
| 8  | print_int | AX = value | — |

Before calling INT 60h, set ES to your module segment (`0x2000`). The `api.h` wrappers handle this.

## Limitations

- No dynamic memory, no malloc, no free
- Flat binary, no relocations, no linker fixups
- File I/O not yet exposed via INT 60h (coming when I get around to it)
- Max module size: 5 sectors (2560 bytes) by default — change `MODULE_SECTORS` in `types.h`
