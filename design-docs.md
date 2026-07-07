# IridiumOS Design Brainstorming
#### ***"Osmium's periodic neighbor"***

Ir repo - https://github.com/untrustedinstaller/iridium
Os repo - https://github.com/untrustedinstaller/osmium

IridiumOS will soon become its own, standalone operating system, only remaining a sibling to OsmiumOS. While Osmium will remain 16-bit real-mode and compacted to a floppy disk.

The user should enjoy using Iridium. Drawn in by its wonder, playfulness, and comfort. The interface should be both foreign and familiar, a new take on an old classic. Iridium should be easy to use, stable, fast, and fun. Iridium should capture the wonder of DIY 90s operating systems such as: early Linux, BeOS, Unix (IRIX, Solaris, etc..), NeXTSTEP while still remaining wholly unique to itself. Iridium should stand out among the crowd, with a unique gimmick or feature that will put it on the map.

### Iridium will be:
- Fully graphical, compact homebrew C implementation of Dear ImGUI with its own design language. Compact doesn't mean sparse — feature-rich, composable, and performant without bloat.
- 32-bit protected mode, with an Osmium compatibility layer/subsystem.
- Much larger scoped than Osmium, small and performant enough to run on a large range of storage mediums, starting with smaller mass-media devices like USB, CD-R, and HDD. Eventually, if not initially, an extremely compact floppy version, to tie back to Osmium.

### User Interface:

#### General tone:
- Quirky, witty, funny, think early esolanguages.
- Boot messages are the main target, things the user will only notice if they Look
- Builtin program names could be references or clever a play on words

##### Builtin program names:
```
Explorer         = Manilla         (manilla folder)
Process viewer   = Fin             (fish theme — light, playful)
Paint            = Easel           (artist's easel)
Text editor      = Quill           (ink and parchment)
Settings         = Configurator    (absurdly formal)
Package manager  = Toybin          (box of toys)
File search      = Nose            (sniffs out files)
Clock            = Clock           (don't overstep)
Disk utility     = Cookiecutter    (cuts out disk shapes)
Screensaver      = Screensaver     (don't overstep)
Archive tool     = Packer          (packs/unpacks)
Sound mixer      = Mixer           (named after the hardware)
Hex editor       = Sextant         ("sex" joke, sextant → navigate bytes)
Hex dump         = Pump & Dump     (continuing the motif)
Image viewer     = Darkroom        (photography darkroom)
```

##### Witty boot messages (candidates):
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

##### Shutdown messages:
Every shutdown displays a randomized, quirky message in place of the classic "It is now safe to turn off your computer." Same amber/orange BIOS-era text, centered on a black screen:
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

#### General Looks:
- Calm, comfortable muted purple accent color (#7845a8), as opposed to Osmium's Windows NT-esque blue (#3B6EA5).
- An amalgam of Motif design language from early Linux/Unix and the Windows 9X/NT Interface
- Pixel-perfect, grey borders and buttons, professional.
- Playful, welcoming, comfortable, with personality, that still remains utilitarian and gets shit done. Think MacOS/BeOS mysticism/whimsy meets Windows 2000--  *(Look into SerenityOS)*  
- Boot screen should go ***ALL IN*** on the 90s OS style, Windows 2000, MacOS 8.x/9.x
  - Full 640×480 VBE mode, centered purple progress bar with Motif 3D bevel
  - Witty boot messages cycle underneath in smaller text
  - At the bottom: "IridiumOS — Osmium's periodic neighbor"
  - Fade/wipe transition into desktop
  - Entirely new startup sound (distinct from Osmium's chime — think Win95 or 2000)

#### Window Manager:
- **Title bars**: Gradient purple (`#5A2E82` → `#7845a8`), with MacOS-style notches at the sides. White title text centered. Close button is a simple `×` in the top-right corner.
- **Window frame**: Classic 3D bevel (white highlight top/left, `#808080` shadow bottom/right). No glow effects — standard active/inactive border coloring.
- **Window content drag**: Dithered border outline while dragging (classic System Software / Windows 3.x style — not live content). On release, the window snaps into place.
- **Minimize/maximize**: macOS Genie-style — the window content shrinks into a small square at the bottom of the screen (expand on restore, shrink on minimize).
- **Desktop**: Clean purple backdrop. Minimal bitmap icons, all hand-drawn, no more than 6-8 on a fresh install. New programs do not automatically create desktop icons.
- **Taskbar**: Thin strip at the bottom, dark grey (`#404040`) or shadow grey (`#808080`). Left side: a small purple diamond or circle with the text "shelf" or "iridium" — this is the Start menu equivalent. Right side: clock. Running apps appear as centered text labels.
- **Start menu**: Click the diamond → pops up a small grey window with: `[Programs]` → submenu, `[Documents]` → submenu of recent files, `[Configurator]`, `[Shutdown]`. Max two levels of nesting.
- **Notifications**: Old-school modal popups (not toasts). Critical errors, disk full, etc. appear as centered dialog boxes with the purple title bar and a grey button ("OK" or "Cancel"). No sliding banners.
- **Right-click everywhere**: Every window, every widget has a context menu. Even the desktop. Desktop right-click → "Change wallpaper" → palette dialog.
- **Button style**: Motif raised 3D bevel with grey face. Mix of SerenityOS's chunky pixel-perfect buttons and classic Motif proportions.

#### Terminal Emulator (Conch):
- Not aggressive or brutalistic, should be just as welcoming and comforting as the rest of the user interface.-- *(similar to the Friendly Interactive SHell- FISH)*
- Easy to understand

### Architecture:

#### OsmiumOS compatibility layer (Osmiridium)
*(Named after the naturally occuring, stable and sturdy alloy between Os and Ir)*
- Similar to the NT subsystem for DOS. Osmium programs should be able to run in a window (terminal emulator more than likely) just like they would on OsmiumOS. 

*Should be easy enough considering OsmiumOS modules and applications are flat binaries*

- Accent color for Osmiridium windows should be the muted blue of 9X, as to indicate it is operating under a different system than IridiumOS.
- Must provide INT60H API translation for the 16-bit real-mode programs

#### System:
- Must be fully POSIX compatible/compliant. Full UNIX command structure, filestructure, userspace.
- If available, implement on a FAT32 filesystem to maintain as much compatibility with modern operating systems as possible. If the user mounts Iridium's storage medium to a seperate, modern operating system (e.g. Windows, Linux, MacOS) the host OS should be able to view, modify, and understand Iridium's data (to an extent.)
*(Iridium's own application programs and data files that may be proprietary should not be attempted to translate)*

---

### Color Palette

#### Greys (Windows 9X / Motif 3D bevel system)

Base greys follow the classic 5-tone bevel system (highlight / face / shadow / dark shadow / background):

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Button highlight | `#FFFFFF` | 255,255,255 | Top/left 3D bevel edge |
| Button face | `#C0C0C0` | 192,192,192 | Button face, panel backgrounds |
| Button shadow | `#808080` | 128,128,128 | Bottom/right 3D bevel edge |
| Dark shadow | `#404040` | 64,64,64 | Outer border of depressed buttons |
| Window bg | `#FFFFFF` | 255,255,255 | Document / list / edit surfaces |
| Window text | `#000000` | 0,0,0 | Body text |
| Disabled text | `#808080` | 128,128,128 | Greyed-out labels |
| Selection bg | `#C0C0C0` | 192,192,192 | Selected list item bg |
| Selection text | `#000000` | 0,0,0 | Selected list item text |
| Desktop bg | `#7845a8` | 120,69,168 | Purple desktop backdrop |

#### Accent (Iridium signature)

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Primary accent | `#7845a8` | 120,69,168 | Desktop bg, title bar active, hyperlinks, selection highlights |
| Accent highlight | `#9B6FD0` | 155,111,208 | Title bar hover, button hover glow |
| Accent shadow | `#5A2E82` | 90,46,130 | Title bar pressed, depressed accent button |
| Accent muted | `#E8DDF5` | 232,221,245 | Subtle hover states, list selection highlight |

#### Osmiridium subsystem accent (DOS-compat blue, to visually distinguish from native Iridium)

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Osmi title bar | `#3B6EA5` | 59,110,165 | Osmium-compat window title bars |
| Osmi highlight | `#6699CC` | 102,153,204 | Osmi hover states |
| Osmi shadow | `#2A4D75` | 42,77,117 | Osmi pressed |

#### Semantic colors

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Error text | `#CC3333` | 204,51,51 | Error dialogs, stderr |
| Warning text | `#CC8800` | 204,136,0 | Warning dialogs |
| Success text | `#338833` | 51,136,51 | Success/OK |
| Info text | `#7845a8` | 120,69,168 | Info messages (uses accent) |

#### Notes
- The 5-tone grey bevel system is lifted straight from Motif/Windows 9X. Buttons render as `highlight` (top/left border), `face` (fill), `shadow` (bottom/right border), with `dark shadow` for the outer edge of depressed/selected buttons. This is the pixel-perfect 3D look.
- The purple accent stays out of the button bevels — greys do the structural work, purple provides identity. Purple appears in: desktop background, active title bars, selection highlights, link text, and the boot screen.
- Osmiridium windows use the Osmium blue instead of purple so the user instantly knows they're in a compatibility layer.
