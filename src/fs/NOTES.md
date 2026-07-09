# Filesystem

## Planned

- FAT32 read/write (for cross-compatibility with Windows/Linux/macOS)
- VFS layer (unified file handles, mount points)
- Open file descriptor table
- POSIX-ish API: `open`, `read`, `write`, `close`, `stat`, `opendir`, `readdir`

## Design notes

- FAT32 is the primary target — it's simple, well-documented, and trivially readable from modern OSes
- A VFS abstraction will allow adding other filesystems later (ISO 9660 for CD-R, ext2 for native)
- Long filename (LFN) support is nice-to-have on FAT32
