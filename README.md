# QchuAnims

A minimal 3D scene engine for visualizing parametric curves, vector math, meshes, and animations — all defined in plain text files.

<p align="center">
  <code>line</code> · <code>quad</code> · <code>curve</code> · <code>mesh</code> · <code>group</code> · <code>preset</code> · <code>animate</code>
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
line x_axis  0 0 0  3 0 0  1 0.3 0.3
curve spiral cos(t) sin(t) t/6  0.2 0.9 0.4
mesh cube1   cube.obj  0.4

set cube1 position 2 0 0
animate cube1 rotation linear linear 0 0 0  0 360 0  4.0
```

Navigate scenes with `←` / `→`. Edit the file — changes appear instantly (hot reload).

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
| `Tab` | Perspective ↔ Orthographic |
| `Q` / `E` | Zoom (ortho) |
| `←` `→` | Cycle scenes |
| `Esc` | Quit |

---

## Project structure

```
assets/
  scenes/          .txt scene files
  textures/        PNG textures
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
