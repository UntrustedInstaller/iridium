# IridiumOS User Interface Specification

**Version:** 0.1  
**Status:** Draft

---

## 1. Desktop

### 1.1 Background

- Solid purple (`#7845a8`) fill
- No tiled wallpaper or image by default
- Right-click context menu: "Change wallpaper" → opens a palette dialog

### 1.2 Icons

- Minimal: 6–8 icons on a fresh install, hand-drawn bitmaps
- New programs do **not** automatically create desktop icons
- Icon layout: left-to-right top-to-bottom grid, non-overlapping
- Icon labels: white text with thin black shadow (for readability on purple)
- Selection: grey (`#C0C0C0`) rounded rect behind icon, black text

---

## 2. Window Manager

### 2.1 Window Frame

A window consists of:

```
┌──────────────────────────────────────────────┐
│  ◀─ notch ──  Title Text  ── notch ─▶  [×]  │  ← Title bar
├──────────────────────────────────────────────┤
│                                              │
│  Client area                                 │
│                                              │
├──────────────────────────────────────────────┤
│  Status bar (optional)                       │
└──────────────────────────────────────────────┘
```

### 2.2 Title Bar

| Element | Specification |
|---------|---------------|
| Height | 18–20 pixels |
| Active bg | Vertical gradient `#5A2E82` → `#7845a8` |
| Inactive bg | Flat `#808080` |
| Active text | White, centered |
| Inactive text | `#C0C0C0`, centered |
| Notches | MacOS-style indentations at left and right edges, ~4px deep |
| Font | System bitmap (8×16), bold |
| Close button | Simple `×` glyph in top-right corner, 16×16 hit area |

### 2.3 Window Frame Bevel

Classic 3D bevel around the entire window:

| Edge | Color (active) | Color (inactive) |
|------|----------------|-------------------|
| Top | `#FFFFFF` | `#FFFFFF` |
| Left | `#FFFFFF` | `#FFFFFF` |
| Bottom | `#808080` | `#808080` |
| Right | `#808080` | `#808080` |

Thickness: 2px (1px highlight + 1px shadow).

### 2.4 Window Operations

| Operation | Behavior |
|-----------|----------|
| **Move** | Click+drag title bar → dithered border outline follows cursor → snap on release (no live content move) |
| **Resize** | Click+drag resize handle → dithered outline → snap on release |
| **Minimize** | Genie effect: window content shrinks into a small square at the bottom of the screen. Click square → genie expands back. |
| **Maximize** | Toggle between full-screen (with taskbar visible) and previous size+position |
| **Close** | Immediate close with optional "Save?" dialog if content is dirty |

### 2.5 Z-Order

- Managed by the compositor
- Click raises window to top
- Active window has colored title bar; others are grey

---

## 3. Taskbar ("Shelf")

### 3.1 Layout

- Position: bottom of screen, full width
- Height: 28 pixels
- Background: dark grey (`#404040`)
- Dividers: thin `#808080` vertical lines between sections

### 3.2 Elements

```
┌──────┬──────────────────────────────────┬──────────┐
│  ◆   │  Manilla    Quill    Easel       │  14:32   │
└──────┴──────────────────────────────────┴──────────┘
  ^                                           ^
  Start/"shelf" button                        Clock
```

### 3.3 Start Button

- A small purple diamond (`◆`) or circle, with text "shelf" or "iridium"
- Click → opens Start menu
- Should be visually distinct from application buttons

### 3.4 Application Buttons

- Centered text labels for each running window
- Active window: highlighted/pressed appearance
- Click → raise window to top (or minimize if already on top)

### 3.5 Clock

- Right-aligned
- Format: `HH:MM` (24-hour or 12-hour configurable)
- Background: transparent (just white text on `#404040`)

---

## 4. Start Menu

### 4.1 Structure

```
┌──────────────────────────┐
│  ◆ Iridium               │
├──────────────────────────┤
│  Programs ──────────────▶│  → Manilla
│  Documents ─────────────▶│  → Recent files
│                           │
│  Configurator            │
│  Shutdown...             │
└──────────────────────────┘
```

- Max two levels of nesting
- Keyboard navigation: arrow keys, Enter, Escape
- Click outside → dismiss

### 4.2 Appearance

- Small grey window (standard Motif bevel)
- Purple title bar with "shelf" text
- No close button (click outside or Escape to dismiss)

---

## 5. Controls & Widgets (IMGUI)

### 5.1 Button

```
┌──────────────────┐
│  ░░░░░░░░░░░░░░  │  ← 1px highlight (#FFFFFF)
│  ░░░ OK ░░░░░░  │  ← face (#C0C0C0)
│  ░░░░░░░░░░░░░░  │  ← 1px shadow (#808080)
└──────────────────┘
   1px dark shadow (#404040)
```

- Standard Motif raised bevel
- Pressed: inverts highlight/shadow
- Disabled: grey text, no bevel emphasis
- Min width: 75px
- Height: 23px (matching SerenityOS/Windows 9X proportions)

### 5.2 Checkbox / Radio Button

- Square (checkbox) or circle (radio), 13×13 pixels
- Face: `#FFFFFF` with Motif sunken bevel
- Check/glyph: purple (`#7845a8`) or black
- Label to the right, 4px gap

### 5.3 Text Input

- Sunken bevel (inverted highlight/shadow)
- Background: `#FFFFFF`
- Text: `#000000`
- Cursor: blinking vertical bar, purple

### 5.4 Scrollbar

- Width: 16px
- Thumb: Motif raised bevel
- Track: Motif sunken bevel
- Arrows: triangular glyphs at each end

### 5.5 Dialog Box

- Centered on screen (or parent window)
- Standard title bar + frame
- One or more buttons at the bottom (right-aligned)
- Tab order: left-to-right, top-to-bottom

---

## 6. Notifications

- **Modal dialogs only** — no sliding banners or toast notifications
- Critical errors, disk full, etc. appear as centered Motif dialog boxes
- Purple title bar (or grey if inactive)
- Grey button(s): "OK", "Cancel", "Retry" as appropriate
- Block the owning window until dismissed

---

## 7. Context Menus

- **Right-click everywhere** — every window and every widget provides a context menu
- Desktop right-click → "Change wallpaper" → palette dialog
- Menu: Motif raised bevel, grey background, black text
- Item height: 18px
- Selected item: purple (`#7845a8`) highlight

---

## 8. Terminal Emulator (Conch)

- Friendly tone, not aggressive or brutalistic
- Similar spirit to the Friendly Interactive SHell (FISH)
- Color scheme: black bg, green/white text (classic), with purple accent for the prompt
- Tab completion, syntax highlighting, clear error messages

---

*This specification is a living document. Implementation may deviate from these descriptions; update this document when intentional deviations occur.*
