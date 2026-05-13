# QchuAnims
> [descripción corta — qué es, para qué sirve, qué lo hace distinto]

![demo gif o screenshot]

---

## What it does
[Explicar en 2-3 oraciones qué puede hacer el motor hoy: renderizar escenas 3D definidas en archivos de texto, curvas paramétricas, cámara libre, etc.]

## How it works

### Scene files
[Explicar que las escenas se definen en `assets/scenes/*.txt`, que cada línea es una entidad, y que todas se cargan al iniciar.]

```
# Example: assets/scenes/main_scene.txt
line  my_line   0 0 0   1 0 0   1 0 0
quad  my_quad   wood.png   2 1
curve my_curve  cos(t)  sin(t)  t/6   0 1 1
```

### Supported entities
| Type  | Description |
|-------|-------------|
| `line`  | [descripción] |
| `quad`  | [descripción] |
| `curve` | [descripción] |

### Parametric curves
[Explicar que `x(t)`, `y(t)`, `z(t)` son expresiones matemáticas evaluadas en runtime. Mencionar las funciones soportadas: sin, cos, sqrt, etc.]

### Camera controls
| Key / Input | Action |
|-------------|--------|
| `W A S D`   | [descripción] |
| `Space`     | [descripción] |
| `Ctrl`      | [descripción] |
| Mouse       | [descripción] |
| `Esc`       | [descripción] |

---

## Building

### Requirements
- [compilador y versión]
- CMake [versión]
- GLFW 3
- [otros]

### Steps
```bash
mkdir build && cd build
cmake ..
cmake --build .
./qchu-anims
```

---

## Project structure
```
assets/scenes/      scene .txt files
assets/textures/    textures referenced in scenes
src/                source files
include/            headers
shaders/            GLSL vertex and fragment shaders
docs/               architecture and class reference
```

---

## Roadmap
- [x] Window + camera
- [x] Lines, quads, curves
- [x] Scene loading from .txt
- [ ] Animation system
- [ ] [lo que sigue]
