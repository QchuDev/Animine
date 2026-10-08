# Animine

A minimal 3D scene engine for visualizing parametric curves, vector math, meshes, and animations — all defined in plain text files.

<p align="center">
  <code>line</code> · <code>quad</code> · <code>curve</code> · <code>mesh</code> · <code>group</code> · <code>preset</code> · <code>background</code> · <code>animate</code> · <code>set</code> · <code>wait</code>
</p>

---

## Quick start

```bash
mkdir build && cd build
cmake .. -G Ninja
cmake --build .
./qchu-anims
```

Create a file in `assets/scenes/`:

```
# assets/scenes/hello.txt
background backgrounds/paper.png
line x_axis  0 0 0  3 0 0  1 0.3 0.3
curve spiral cos(t) sin(t) t/6  0.2 0.9 0.4
mesh cube1   cube.obj  0.4

set cube1 position 2 0 0
animate cube1 rotation linear linear 0 0 0  0 360 0  4.0
```

Navigate scenes with `←` / `→`. Edit the file — changes appear instantly (hot reload).

---

## Scene commands

Each scene is a plain-text file in `assets/scenes/`. Lines starting with `#` are comments.

### Entities

| Command | Syntax |
|---------|--------|
| `line`  | `line <id> <x1 y1 z1> <x2 y2 z2> <r g b> [stroke]` |
| `quad`  | `quad <id> <texture> [w h \| 12 corner coords]` |
| `curve` | `curve <id> <x_expr> <y_expr> <z_expr> <r g b> [t_min t_max] [stroke]` |
| `mesh`  | `mesh <id> <file.obj> [texture] [scale]` |

Expressions for `curve` use the variable `t` (e.g. `cos(t)`, `sin(t)`, `t/6`).

### Grouping

| Command  | Syntax |
|----------|--------|
| `group`  | `group <id> <child_id> <child_id> ...` |
| `preset` | `preset <id> <file>` — loads a reusable sub-scene from `assets/presets/` |

### Environment

| Command      | Syntax |
|--------------|--------|
| `background` | `background <texture>` — fullscreen background, drawn in screen-space (does not move with the camera). Path is relative to `assets/textures/`. |

### Timeline

| Command   | Syntax |
|-----------|--------|
| `set`     | `set <id> <property> <x y z>` — instantly sets a property at the current time |
| `wait`    | `wait <seconds>` — advances the timeline cursor for subsequent commands |
| `animate` | `animate <id> <property> <easing> <interp> <args> <duration>` |

- **property**: `position`, `rotation`, `scale`, `color`
- **easing**: `linear`, `ease_in`, `ease_out`, `ease_in_out`
- **interp**: `linear`, `smooth`, `path`
  - For `linear` / `smooth`: `<args>` is a list of waypoints `x y z ...`
  - For `path`: `<args>` is three expressions `<x_expr> <y_expr> <z_expr>` (using `t`)

---

## Documentation

| Document | Contents |
|----------|----------|
| [Entities](docs/entities.md) | Line, Quad, Curve, Mesh — syntax, parameters, examples |
| [Animations](docs/animations.md) | animate, set, wait — easing, interpolation, timeline |
| [Groups & Presets](docs/groups-and-presets.md) | Hierarchical transforms, reusable sub-scenes |

---

## Controls

| Input | Action |
|-------|--------|
| `W A S D` | Move |
| `Space` / `Ctrl` | Up / Down |
| Mouse | Look around |
| `G` | Toggle GUI mode (cursor visible; camera moves only while holding right-click) |
| `Tab` | Perspective ↔ Orthographic |
| `Q` / `E` | Zoom (ortho) |
| `←` `→` | Cycle scenes |
| `Esc` | Quit |

---

## Project structure

```
assets/
  scenes/          .txt scene files
  textures/        PNG textures (strokes/, backgrounds/, ...)
  meshes/          .obj models
  presets/         reusable sub-scenes
src/
  classes/
    engine.cpp     main loop, input, hot reload
    render/        renderer, shader, camera
    entities/      line, quad, curve, mesh, group
    animations/    animator, tracks, easing
    creation/      scene parser
    scenes/        scene manager
include/
  classes/         headers
  external/        tinyexpr, tinyobjloader
shaders/           GLSL shaders
```

---

## Requirements

- C++17 compiler
- CMake ≥ 3.10
- GLFW 3
- OpenGL

Dependencies vendored: glad, glm, stb_image, tinyexpr, tinyobjloader.

---

## Roadmap

- [x] Lines, quads, parametric curves
- [x] 3D mesh loading (.obj)
- [x] Animation system (easing, waypoints, parametric paths)
- [x] Color animation
- [x] Entity groups & presets
- [x] Hot reload
- [ ] Editor / GUI
