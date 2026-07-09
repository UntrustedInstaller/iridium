# IridiumOS Milestones

**Phase 0 — Bootstrap** ← YOU ARE HERE

- [x] Multiboot kernel that boots in QEMU `-kernel`
- [x] VGA text mode output with scrolling
- [x] Interactive build script with dependency checking
- [x] Design docs, color palette, UI spec formalized

---

**Phase 1 — Foundation** (~next)

- [ ] Own GDT (bootloader gives us one, but we should own it)
- [ ] IDT with handler stubs (divide-by-zero, GPF, etc.)
- [ ] PIC remapping and IRQ handlers
- [ ] PIT timer (system tick, 100 Hz or 1000 Hz)
- [ ] PS/2 keyboard driver (scancode set 1 → ASCII + keycodes)
- [ ] Ring buffer input queue
- [ ] Simple echo shell (`kernel_main` → shell loop)

---

**Phase 2 — Memory & Processes**

- [ ] Memory detection (multiboot info or INT 0x15 E820)
- [ ] Frame allocator (physical page allocator, bitmap or stack)
- [ ] Paging (identity map first 4 MB + remap kernel high)
- [ ] Heap allocator (`kmalloc` / `kfree` based on paging)
- [ ] Multitasking: thread context switch (timer IRQ)
- [ ] User mode: TSS, rings, syscall gate

---

**Phase 3 — Drivers & Filesystem**

- [ ] IDE PATA driver (polling, then DMA)
- [ ] FAT32 read (BPB parsing, cluster chain traversal)
- [ ] FAT32 write (cluster allocation, directory entry creation)
- [ ] VFS abstraction (mount points, file handles)
- [ ] `open` / `read` / `write` / `close` syscalls
- [ ] El Torito CD-R support (for ISO boot)

---

**Phase 4 — Shell & Userland**

- [ ] Conch (terminal emulator with FISH-like friendliness)
- [ ] Command autocomplete
- [ ] PATH-based program execution
- [ ] Background processes (`&`)
- [ ] Pipes and redirection

---

**Phase 5 — GUI**

- [ ] VBE framebuffer mode set (640×480 32bpp)
- [ ] IMGUI core: input state, widget layout, clip rects
- [ ] Widgets: button, label, checkbox, textbox, scrollbar
- [ ] Motif bevel renderer (highlight/face/shadow/dark-shadow)
- [ ] Font renderer (8×16 bitmap glyphs + blitter)
- [ ] Compositor: window surface, damage tracking, blit
- [ ] Window manager: move (dither outline), resize, z-order
- [ ] Title bar (gradient purple, notches, close button)
- [ ] Taskbar ("shelf") with running apps and clock
- [ ] Start menu (programs, documents, configurator, shutdown)
- [ ] Desktop icons (hand-drawn bitmaps)
- [ ] Right-click context menus everywhere

---

**Phase 6 — Built-in Applications**

- [ ] Manilla (file explorer)
- [ ] Quill (text editor)
- [ ] Easel (paint)
- [ ] Configurator (settings)
- [ ] Fin (process viewer)
- [ ] Toybin (package manager)
- [ ] Darkroom (image viewer)
- [ ] Sextant (hex editor)
- [ ] Clock
- [ ] Cookiecutter (disk utility)

---

**Phase 7 — Osmiridium Subsystem**

- [ ] Osmium 16-bit flat binary loader
- [ ] `int 0x60` → native syscall translation table
- [ ] Osmiridium terminal window (blue title bar)
- [ ] Compat testing with Osmium BASIC, editor, bf

---

**Phase 8 — Polish & Stretch**

- [ ] Boot screen (VBE, progress bar, witty messages cycling)
- [ ] Startup sound
- [ ] Shutdown screen (randomized messages, amber text)
- [ ] Genie minimize effect
- [ ] Wallpaper/theme support
- [ ] Floppy disk variant (stretch)
- [ ] Network stack (stretch — ne2000 or RTL8139)
