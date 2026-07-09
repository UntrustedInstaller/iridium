# Library

Freestanding runtime — no libc available. Implement what we need.

## Planned

- `memset`, `memcpy`, `memmove`, `memcmp`
- `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat`
- `vsnprintf` / `sprintf` / `printf` (for kernel logging)
- `atoi`, `itoa` (for shell)
- Linked list / ring buffer utilities

## Convention

- Prefix kernel-internal functions with `k_` to avoid name clashes
- Keep it compact — no heavy formatting, no floating point
