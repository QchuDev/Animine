# QchuAnims

A minimal 3D scene engine for visualizing parametric curves, vector math, and animations — all defined in plain text files.

---

## What it does

Renders 3D scenes defined entirely in `.txt` files: lines, textured quads, and parametric curves with math expressions evaluated at runtime. Includes a timeline-based animation system that can move, rotate, and scale entities along waypoints or parametric paths with easing functions. Navigate scenes with a free-fly camera and cycle between them with arrow keys.

## How it works

### Scene files

Scenes live in `assets/scenes/` as `.txt` files. Each file becomes a scene. Lines starting with `#` are comments. The engine loads all scenes on startup and lets you cycle through them with `←` / `→`.

A scene file mixes entity declarations and animation commands:

```
# assets/scenes/example.txt

# Entities
line  my_line   0 0 0   1 0 0   1 0 0
quad  my_quad   wood.png  2 1
curve helix     cos(t)  sin(t)  t/6   0 1 1

# Animation
set my_quad position 0 -1 0
animate my_quad position ease_out path t*2 sin(t*3) 0 4.0
wait 4.0
animate my_line rotation linear smooth 0 0 90 0 0 180 2.0
```

### Supported entities

| Type    | Syntax | Description |
|---------|--------|-------------|
| `line`  | `line <id> <x1 y1 z1> <x2 y2 z2> <r g b> [stroke_name]` | A colored line segment between two 3D points. Color is RGB normalized (0–1). Optional stroke texture from `assets/textures/strokes/`. |
| `quad`  | `quad <id> <texture> [w h]` or `quad <id> <texture> <12 floats>` | A textured quad. Specify width/height for a centered rectangle, or 4 explicit vertices (12 floats). |
| `curve` | `curve <id> <x(t)> <y(t)> <z(t)> <r g b> [t_min t_max] [stroke_name]` | A parametric curve. Expressions are evaluated with `t` using tinyexpr (supports `sin`, `cos`, `tan`, `sqrt`, `exp`, `log`, `pi`, `e`, arithmetic, and parentheses). Range defaults to [0, 2π] if omitted. |

### Scene commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `background` | `background <texture>` | Set a fullscreen background texture (screen-space, fixed) |

### Animation system

Animations are declared inline after entity definitions:

| Command | Syntax | Description |
|---------|--------|-------------|
| `animate` | `animate <id> <property> <easing> <interp> <args> <duration>` | Animate an entity's transform over time |
| `wait` | `wait <seconds>` | Offset the start time of subsequent animations |
| `set` | `set <id> <property> <x y z>` | Instantly set a transform property at the current timeline position |

**Properties:** `position`, `rotation`, `scale`

**Easing:** `linear`, `ease_in`, `ease_out`, `ease_in_out`

**Interpolation modes:**
- `linear` — lerp between waypoints: `<x y z> [<x y z>...] <duration>`
- `smooth` — smooth interpolation between waypoints (same syntax as linear)
- `path` — parametric path with expressions: `<x(t)> <y(t)> <z(t)> <duration>` where `t` goes from 0 to 1

### Camera controls

| Key / Input | Action |
|-------------|--------|
| `W A S D` | Move forward / left / backward / right |
| `Space` | Move up |
| `Left Ctrl` | Move down |
| Mouse | Look around (yaw/pitch) |
| `←` `→` | Previous / next scene |
| `Esc` | Close window |

---

## Building

### Requirements

- MSVC (C++17) or any C++17 compiler
- CMake ≥ 3.10
- GLFW 3 (found via `find_package`)
- OpenGL

All other dependencies (glad, glm, stb_image, tinyexpr) are vendored in `include/`.

### Steps

```bash
mkdir build && cd build
cmake .. -G Ninja
cmake --build .
./qchu-anims
```

The build copies shaders to the output directory automatically.

---

## Project structure

```
assets/scenes/       scene .txt files (one scene per file)
assets/textures/     textures referenced by quads (PNG)
src/                 source files (main, glad, tinyexpr, stb_image)
src/classes/         engine, entities, renderer, animator, parser
include/classes/     headers for all classes
include/external/    tinyexpr header
shaders/             GLSL vertex and fragment shaders
docs/                architecture notes and animation system design
```

---

## Roadmap

- [x] Window + free-fly camera
- [x] Lines, textured quads, parametric curves
- [x] Scene loading from `.txt` files
- [x] Scene cycling (← →)
- [x] Animation system (tracks, keyframes, easing, path interpolation)
- [x] `set` / `wait` timeline commands
- [ ] Editor / GUI
- [ ] More entity types
