# Entities

Entities are the building blocks of a scene. Each entity has a unique `id`, a transform (position, rotation, scale), and a color.

---

## Line

A colored line segment between two 3D points, rendered as a camera-facing quad with a stroke texture.

```
line <id> <x1 y1 z1> <x2 y2 z2> <r g b> [stroke]
```

| Parameter | Description |
|-----------|-------------|
| `id` | Unique identifier |
| `x1 y1 z1` | Start point |
| `x2 y2 z2` | End point |
| `r g b` | Color (normalized 0–1) |
| `stroke` | Optional. Texture name from `assets/textures/strokes/` (without path or `.png`) |

```
line x_axis  0 0 0  3 0 0  1 0.3 0.3
line fancy   0 0 0  0 2 0  0.2 0.8 1.0  brush
```

---

## Quad

A textured rectangle or arbitrary quadrilateral.

```
quad <id> <texture> [w h]
quad <id> <texture> <x1 y1 z1 x2 y2 z2 x3 y3 z3 x4 y4 z4>
```

| Parameter | Description |
|-----------|-------------|
| `id` | Unique identifier |
| `texture` | Filename from `assets/textures/` |
| `w h` | Width and height (centered at origin) |
| 12 floats | 4 explicit vertex positions |

```
quad bg       sky.png  10 6
quad panel    wood.png  2 1
quad custom   tile.png  -1 0 0  1 0 0  1 1 0  -1 1 0
```

---

## Curve

A parametric curve defined by math expressions in `t`.

```
curve <id> <x(t)> <y(t)> <z(t)> <r g b> [t_min t_max] [stroke]
```

| Parameter | Description |
|-----------|-------------|
| `id` | Unique identifier |
| `x(t) y(t) z(t)` | Expressions using `t`. Supports: `sin`, `cos`, `tan`, `sqrt`, `exp`, `log`, `pi`, `e`, `^`, `()` |
| `r g b` | Color (normalized 0–1) |
| `t_min t_max` | Optional range. Defaults to `0` – `6.2832` (2π) |
| `stroke` | Optional stroke texture |

```
curve helix    cos(t)  sin(t)  t/6        0.2 0.9 0.4
curve parabola t       t^2     0          1.0 0.5 0.0  0 2
curve wave     t       sin(t*3)*0.5  0    0.3 0.6 1.0  0 6.28  soft
```

---

## Mesh

A 3D model loaded from a Wavefront `.obj` file, rendered with diffuse lighting.

```
mesh <id> <file.obj> [texture] [scale]
```

| Parameter | Description |
|-----------|-------------|
| `id` | Unique identifier |
| `file.obj` | Filename from `assets/meshes/` |
| `texture` | Optional. Filename from `assets/textures/` |
| `scale` | Optional float. Uniform scale at load time (default `1.0`) |

```
mesh monkey  suzanne.obj
mesh cube1   cube.obj  0.5
mesh floor   plane.obj  tiles.png
mesh rock    rock.obj   stone.png  0.3
```

### Details

- Supports vertex positions, normals, and UV coordinates
- Without texture: renders with solid color (white by default, animable via `color`)
- With texture: sampled using the mesh's UVs
- Lighting: directional light with ambient (0.3) + diffuse (0.7)
- `.mtl` files are ignored — assign textures explicitly in the scene file
- Place `.obj` files in `assets/meshes/`

---

## Common properties

All entities share these animable properties:

| Property | Type | Description |
|----------|------|-------------|
| `position` | `vec3` | World position (x, y, z) |
| `rotation` | `vec3` | Euler angles in degrees (x, y, z) |
| `scale` | `vec3` | Scale factor per axis |
| `color` | `vec3` | RGB color (0–1). For lines/curves: line color. For meshes: tint. |
