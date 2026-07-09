# IridiumOS Color Palette Reference

**Version:** 0.1  
**Status:** Draft

---

## 1. Grey System (Motif / Windows 9X Bevel)

The five-tone bevel system is lifted from Motif and Windows 9X. Buttons, frames, and panels use these greys for the 3D raised/sunken look:

```
     Highlight   ─────┐
     Face        ─────┤  Button or panel
     Shadow      ─────┤
     Dark Shadow ─────┘
     Background        ← behind the bevel
```

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Button highlight | `#FFFFFF` | 255,255,255 | Top/left 3D bevel edge (raised) |
| Button face | `#C0C0C0` | 192,192,192 | Button face, panel backgrounds |
| Button shadow | `#808080` | 128,128,128 | Bottom/right 3D bevel edge (raised) |
| Dark shadow | `#404040` | 64,64,64 | Outer border of depressed buttons |
| Window background | `#FFFFFF` | 255,255,255 | Document/list/edit surfaces |
| Window text | `#000000` | 0,0,0 | Body text |
| Disabled text | `#808080` | 128,128,128 | Greyed-out labels and controls |
| Selection bg | `#C0C0C0` | 192,192,192 | Selected list item background |
| Selection text | `#000000` | 0,0,0 | Selected list item text |
| Desktop bg | `#7845a8` | 120,69,168 | Purple desktop backdrop |

### Bevel rendering

A raised button is drawn as:

```
┌─────────────────────┐
│ 1px highlight       │
│  ┌───────────────┐  │
│  │ face          │  │
│  │               │  │
│  └───────────────┘  │
│ 1px shadow          │
└─────────────────────┘
1px dark shadow
```

A depressed button inverts highlight and shadow.

---

## 2. Purple Accent (Iridium Signature)

Purple provides identity. It appears in: desktop background, active title bars, selection highlights, hyperlinks, and the boot screen. Purple does **not** appear in button bevels — greys do that job.

### Primary palette

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Primary accent | `#7845a8` | 120,69,168 | Desktop bg, active title bars, hyperlinks, selection highlights |
| Accent highlight | `#9B6FD0` | 155,111,208 | Title bar hover, button hover glow |
| Accent shadow | `#5A2E82` | 90,46,130 | Title bar pressed, depressed accent button |
| Accent muted | `#E8DDF5` | 232,221,245 | Subtle hover states, list selection highlight |

### Title bar gradient

Active title bars should use a vertical gradient from `#5A2E82` (top) to `#7845a8` (bottom):

```
┌─────────────────────────┐
│ ░░░░░░░░░░░░░░░░░░░░░  │  ← #5A2E82
│ ░░░░░░░░░░░░░░░░░░░░░  │
│ ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  │  ← #7845a8
└─────────────────────────┘
    Title bar (active)
```

Inactive title bars use a flat `#808080` (grey) background with grey text.

---

## 3. Osmiridium Subsystem Accent (Compat Blue)

Osmiridium windows use Osmium blue instead of Iridium purple so the user instantly knows they are in the compatibility layer.

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Osmi title bar | `#3B6EA5` | 59,110,165 | Osmium-compat window title bars |
| Osmi highlight | `#6699CC` | 102,153,204 | Hover states |
| Osmi shadow | `#2A4D75` | 42,77,117 | Pressed states |

---

## 4. Semantic Colors

| Role | Hex | RGB | Use |
|------|-----|-----|-----|
| Error text | `#CC3333` | 204,51,51 | Error dialogs, stderr output |
| Warning text | `#CC8800` | 204,136,0 | Warning dialogs |
| Success text | `#338833` | 51,136,51 | Success messages |
| Info text | `#7845a8` | 120,69,168 | Info messages (uses primary accent) |

---

## 5. Usage Rules

1. **Buttons use pure greys.** Purple never appears in button bevels or button faces.
2. **Title bars use purple (active) or grey (inactive).** The gradient is vertical, darker at top.
3. **Selection highlights** default to grey (`#C0C0C0`), matching Motif conventions.
4. **Osmiridium windows** swap purple for blue across all accent roles.
5. **Text is black on white** for document/editing surfaces — readability first.
6. **Disabled elements** use grey text (`#808080`) on face-grey (`#C0C0C0`) — no color shift.

---

*This palette is a living reference. Add new colors only when a clear design need arises; prefer using existing palette entries whenever possible.*
