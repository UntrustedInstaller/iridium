# IridiumOS Design Philosophy

**Version:** 0.1  
**Status:** Draft — subject to change  
**Tagline:** *"Osmium's periodic neighbor"*

---

## 1. Core Philosophy

IridiumOS should be **enjoyable to use**. The user is drawn in by wonder, playfulness, and comfort. The interface is both foreign and familiar — a new take on an old classic. Iridium is:

- **Easy to use** — discoverable, predictable, consistent.
- **Stable** — crashes are exceptional, not expected.
- **Fast** — responds instantly; never wastes the user's time.
- **Fun** — wit and personality are features, not bugs.

The reference points are the golden age of DIY operating systems (early Linux, BeOS, IRIX, Solaris, NeXTSTEP) — but Iridium is not a clone of any of them. It synthesizes their best ideas into something **wholly unique**.

A defining gimmick or feature should put it on the map — something that makes a user say *"only Iridium does that."*

---

## 2. Scope & Platform

| Dimension | Target |
|-----------|--------|
| **CPU mode** | 32-bit protected mode (i586+) |
| **Minimum RAM** | 16 MB (target), 4 MB (stretch) |
| **Storage** | USB, CD-R, HDD; floppy variant as stretch goal |
| **Graphical output** | VGA text mode (early), VBE 640×480 or 1024×768 (full GUI) |
| **Filesystem** | ext2 native; FAT32 read/write for host OS interoperability |
| **Compatibility** | POSIX-compliant userland; OsmiumOS 16-bit subsystem (Osmiridium) |

---

## 3. User Interface

See also: [UI Specification](UI.md)

### 3.1 Tone

The OS speaks to the user with **quirky, witty, and warm personality**. The primary targets are:

- **Boot messages** — easter eggs for anyone who watches the screen during boot
- **Dialog text** — error messages that don't blame the user
- **Built-in program names** — clever wordplay instead of generic labels
- **Shutdown messages** — randomized send-offs that replace the sterile "safe to turn off"

### 3.2 Visual Language

| Influence | Element |
|-----------|---------|
| **Motif** (CDE, IRIX) | 3D bevel system for buttons, frames, and panels |
| **Windows 9X/NT** | Taskbar, title bars, desktop metaphor |
| **Mac OS 8/9** | Platinum aesthetics, title bar notches, genie minimize |
| **BeOS** | Responsiveness, workspace concept, clean layout |
| **SerenityOS** | Pixel-perfect proportions, chunky buttons |

The result is a **grey-and-purple** palette that feels professional yet playful:

- **Greys** do the structural work (bevels, frames, backgrounds)
- **Purple** provides identity (title bars, desktop, selections, accents)

See [Color Palette Reference](COLORS.md) for the full specification.

### 3.3 Boot Experience

The boot screen goes *all in* on the 90s OS aesthetic — think Windows 2000 startup meets Mac OS 9 splash:

- Full 640×480 VBE mode
- Centered purple progress bar with Motif 3D bevel
- Cycle through witty boot messages in smaller text below the bar
- Footer: "IridiumOS — Osmium's periodic neighbor"
- Fade or wipe transition into the desktop
- Distinct startup sound (different from Osmium's chime; evokes Win95 or 2000)

---

## 4. Architecture

### 4.1 System Model

- **Flat memory model** with paging (4 KB pages)
- **Monolithic kernel** with loadable modules
- **Preemptive multitasking** (round-robin or priority)
- **POSIX-compliant system calls** for userland
- **IMGUI-based compositor** drives all rendering

### 4.2 Compatibility: Osmiridium Subsystem

The Osmiridium subsystem provides an **OS/2-like compatibility layer** for running 16-bit real-mode OsmiumOS programs:

- Named after the naturally occurring, stable alloy of Osmium and Iridium
- Runs Osmium flat binaries in a terminal emulator window
- Translates `int 0x60` API calls to native Iridium system calls
- Osmiridium windows use the **Osmium blue** (`#3B6EA5`) title bar instead of Iridium purple, so the user instantly recognizes the compatibility context

### 4.3 Filesystem

- **ext2** is the native filesystem — simple, well-documented, natively Unix (permissions, hard links, symlinks, device nodes)
- **FAT32** is supported as a secondary read/write driver for cross-platform interoperability (USB sticks, shared drives)
- A mounted Iridium drive should be readable and writable from Windows, Linux, and macOS (via the FAT32 driver or ext2 tools)
- Iridium-specific metadata and program data uses conventional file formats where possible; proprietary formats are the exception, not the rule
- VFS abstraction will support additional filesystem types in the future

### 4.4 Programs

| Program | Name | Notes |
|---------|------|-------|
| File explorer | Manilla | Manilla folder wordplay |
| Process viewer | Fin | Fish theme — light, playful |
| Paint | Easel | Artist's easel |
| Text editor | Quill | Ink and parchment |
| Settings | Configurator | Absurdly formal |
| Package manager | Toybin | Box of toys |
| File search | Nose | Sniffs out files |
| Clock | Clock | Don't overstep |
| Disk utility | Cookiecutter | Cuts out disk shapes |
| Screensaver | Screensaver | Don't overstep |
| Archive tool | Packer | Packs/unpacks |
| Sound mixer | Mixer | Named after the hardware |
| Hex editor | Sextant | "Sex" joke; navigate bytes |
| Hex dump | Pump & Dump | Continuing the motif |
| Image viewer | Darkroom | Photography darkroom |
| Terminal | Conch | Friendly, not aggressive |

---

## 5. Boot Messages

### 5.1 Startup

Cycled during boot screen phase:

```
Pouring 0x0D cups of coffee...
Counting RAM... 639. 640. Wait. 639.
Making sure the purple is purple enough. It is.
Spinning up the thing that spins up things...
Error 0x00000000: Everything is fine.
Your Iridium is now 47% more iridium.
Mounting filesystem... it's around here somewhere.
Loading shell. It's a nice shell. You'll like it.
Checking for dragons... none found.
Unbending the paperclips...
```

### 5.2 Shutdown

Randomly selected and displayed as amber/orange text centered on a black screen, in place of "It is now safe to turn off your computer":

```
It is now safe to turn off your Iridium.
Don't forget to write.
Going home. Catch you tomorrow.
Remember to feed the oscillators.
Your Iridium is now 73% less active.
Spinning down the thing that spins up things...
Be excellent to each other.
The cats are back in their carriers.
See you next time.
```

---

## 6. File Structure Conventions

- **Kernel** source files live in `src/kernel/`
- **Drivers** live in `src/drivers/`, one file per device class
- **Library** functions live in `src/lib/` (freestanding; no libc dependency)
- **GUI toolkit** lives in `src/gui/`
- **Modules** (loadable programs) live in `src/modules/`
- **Documentation** lives in `docs/` — markdown, no build artifacts

---

*This document captures the design intent. Implementation decisions may diverge as practical constraints emerge; revisit and update as the project evolves.*
