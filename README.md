# QchuAnims

A minimal 3D scene engine for visualizing parametric curves, vector math, meshes, and animations — all defined in plain text files.

---

## What it does

Renders 3D scenes defined entirely in `.txt` files: lines, textured quads, parametric curves, and 3D meshes with math expressions evaluated at runtime. Includes a timeline-based animation system that can move, rotate, scale, and recolor entities along waypoints or parametric paths with easing functions. Supports entity grouping for hierarchical transforms and reusable presets. Navigate scenes with a free-fly camera and cycle between them with arrow keys.

---

## Scene files

Scenes live in `assets/scenes/` as `.txt` files. Each file becomes a scene. Lines starting with `#` are comments. The engine loads all scenes on startup and lets you cycle through them with `←` / `→`.

```
# assets/scenes/example.txt

# Entities
line   my_line   0 0 0   1 0 0   1 0 0
quad   my_quad   wood.png  2 1
curve  helix     cos(t)  sin(t)  t/6   0 1 1
mesh   cube1     cube.obj  0.5

# Grouping
group my_group  my_line my_quad

# Animation
set my_quad position 0 -1 0
animate my_group position ease_out path t*2 sin(t*3) 0 4.0
wait 4.0
animate cube1 rotation linear smooth 0 0 0 0 360 0 3.0
```

---

## Entities

| Type | Syntax | Description |
|------|--------|-------------|
| `line` | `line <id> <x1 y1 z1> <x2 y2 z2> <r g b> [stroke]` | Colored line segment. RGB normalized (0–1). Optional stroke texture from `assets/textures/strokes/`. |
| `quad` | `quad <id> <texture> [w h]` | Textured quad. Width/height for centered rectangle, or 12 floats for 4 explicit vertices. |
| `curve` | `curve <id> <x(t)> <y(t)> <z(t)> <r g b> [t_min t_max] [stroke]` | Parametric curve. Expressions use `t` via tinyexpr (`sin`, `cos`, `sqrt`, `pi`, etc). Range defaults to [0, 2π]. |
| `mesh` | `mesh <id> <file.obj> [texture] [scale]` | 3D mesh loaded from `.obj` file. Rendered with diffuse lighting. |

### Mesh details

- Files live in `assets/meshes/`
- Supports positions, normals, and UVs from the `.obj`
- `scale` (float) uniformly scales the mesh at load time — e.g. `0.5` = half size
- Without a texture, the mesh renders with a solid color (white by default, animable via `color`)
- With a texture, it's sampled using the mesh's UV coordinates
- Lighting: single directional light with ambient (0.3) + diffuse (0.7)

```
mesh monkey  suzanne.obj                 # no texture, scale 1.0
mesh cube1   cube.obj  0.5              # no texture, scale 0.5
mesh floor   plane.obj  tiles.png       # textured, scale 1.0
mesh rock    rock.obj   stone.png  0.3  # textured, scale 0.3
```

---

## Groups and presets

### Groups

Group entities to animate them as a single unit. Children inherit the group's transform at draw time while keeping their own local transforms.

```
line a  0 0 0  1 0 0  1 0 0
line b  0 0 0  0 1 0  0 1 0
group my_axes  a b

animate my_axes rotation ease_out linear 0 0 0 0 0 360 3.0
animate my_axes color ease_in linear 1 0 0 2.0
```

- Animating `color` on a group propagates to all children
- Animating `position`, `rotation`, `scale` affects the group transform (children move with it)
- Children can still be animated individually

### Presets

Reusable sub-scenes loaded from `assets/presets/`. A preset creates a group automatically with prefixed child IDs to avoid name collisions.

```
# assets/presets/axes.txt
line x  0 0 0  1 0 0  1 0 0
line y  0 0 0  0 1 0  0 1 0
line z  0 0 0  0 0 1  0 0 1
```

```
# In a scene file
preset my_axes  axes.txt

animate my_axes position ease_out linear 0 0 0 2 0 0 1.5
```

Child entities are accessible as `my_axes.x`, `my_axes.y`, `my_axes.z`.

---

## Scene commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `background` | `background <texture>` | Fullscreen background texture (screen-space, fixed) |
| `group` | `group <id> <child1> <child2> ...` | Group existing entities under a shared transform |
| `preset` | `preset <id> <file.txt>` | Load a preset from `assets/presets/` as a group |

---

## Animation system

Animations are declared inline after entity definitions:

| Command | Syntax | Description |
|---------|--------|-------------|
| `animate` | `animate <id> <property> <easing> <interp> <args> <duration>` | Animate a property over time |
| `wait` | `wait <seconds>` | Offset the start time of subsequent animations |
| `set` | `set <id> <property> <x y z>` | Instantly set a property at the current timeline position |

**Properties:** `position`, `rotation`, `scale`, `color`

**Easing:** `linear`, `ease_in`, `ease_out`, `ease_in_out`

**Interpolation modes:**
- `linear` — lerp between waypoints: `<x y z> [<x y z>...] <duration>`
- `smooth` — smooth interpolation between waypoints (same syntax)
- `path` — parametric path: `<x(t)> <y(t)> <z(t)> <duration>` where `t` ∈ [0, 1]

---

## Camera controls

| Key / Input | Action |
|-------------|--------|
| `W A S D` | Move forward / left / backward / right |
| `Space` | Move up |
| `Left Ctrl` | Move down |
| Mouse | Look around (yaw/pitch) |
| `Tab` | Toggle perspective / orthographic |
| `Q` / `E` | Zoom out / in (orthographic mode) |
| `←` `→` | Previous / next scene |
| `Esc` | Close window |

---

## Building

### Requirements

- C++17 compiler (MSVC, GCC, Clang)
- CMake ≥ 3.10
- GLFW 3 (found via `find_package`)
- OpenGL

All other dependencies are vendored: glad, glm, stb_image, tinyexpr, tinyobjloader.

### Steps

```bash
mkdir build && cd build
cmake .. -G Ninja
cmake --build .
./qchu-anims
```

Shaders are copied to the output directory automatically.

---

## Project structure

```
assets/
  scenes/            scene .txt files (one per scene)
  textures/          PNG textures for quads and meshes
  textures/strokes/  stroke textures for lines and curves
  meshes/            .obj files for mesh entities
  presets/           reusable sub-scene .txt files
src/
  main.cpp           entry point
  glad.c             OpenGL loader
  tinyexpr.c         math expression parser
  stb_image.cpp      image loading
  classes/
    engine.cpp       main loop, input, hot reload
    render/          renderer, shader, camera
    entities/        line, quad, curve, mesh, group
    animations/      animator, tracks, keyframes, easing
    creation/        scene parser
    scenes/          scene, scenes_manager
include/
  classes/           headers for all engine classes
  external/          tinyexpr, tinyobjloader, exprtk
shaders/             GLSL vertex and fragment shaders
docs/                architecture notes, implementation plan
```

---

## Roadmap

- [x] Window + free-fly camera
- [x] Lines, textured quads, parametric curves
- [x] Scene loading from `.txt` files
- [x] Scene cycling (← →)
- [x] Animation system (tracks, keyframes, easing, path interpolation)
- [x] `set` / `wait` timeline commands
- [x] Color animation
- [x] Entity groups with hierarchical transforms
- [x] Reusable presets
- [x] 3D mesh loading (.obj) with diffuse lighting
- [ ] Hot reload (file watching)
- [ ] Editor / GUI
