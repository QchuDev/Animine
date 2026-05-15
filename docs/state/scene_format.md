# QchuAnims — Scene File Format

**Location:** `assets/scenes/*.txt`  
**Encoding:** UTF-8, one declaration per line  
**Comments:** lines starting with `#` are ignored  
**IDs:** must be unique within a scene, no spaces

---

## General syntax

```
<type> <id> <args...>
```

---

## Entities

### LINE

```
line <id> <x1> <y1> <z1> <x2> <y2> <z2> <r> <g> <b> [stroke_name]
```

Draws a textured line (billboard quad) from point 1 to point 2. Color is RGB in [0.0, 1.0].  
Optional `stroke_name` loads `assets/textures/strokes/<stroke_name>.png`. Defaults to `line_default` if omitted.

```
line axis_x  0 0 0  1 0 0  1 0 0
line dashed  0 0 0  3 0 0  0.5 0.5 0.5  dotted
```

---

### QUAD

```
# 4 explicit vertices (counter-clockwise: bottom-left, bottom-right, top-right, top-left)
quad <id> <texture> <x1> <y1> <z1> <x2> <y2> <z2> <x3> <y3> <z3> <x4> <y4> <z4>

# Centered at origin, width × height
quad <id> <texture> <w> <h>

# Default 1×1 centered at origin
quad <id> <texture>
```

Texture is a filename relative to `assets/textures/`.

```
quad bg    wood.png  2 1
quad logo  pixels.png  0 0 0  1 0 0  1 1 0  0 1 0
```

---

### CURVE

```
curve <id> <x(t)> <y(t)> <z(t)> <r> <g> <b> [t_min t_max] [stroke_name]
```

Parametric curve. `t` runs from `t_min` to `t_max` (default: `0` to `2π` if omitted).  
Optional `stroke_name` loads `assets/textures/strokes/<stroke_name>.png`. Defaults to `line_default` if omitted.  
Expressions evaluated by tinyexpr.  
Supported: `+` `-` `*` `/` `^` `sin` `cos` `tan` `sqrt` `abs` `log` `exp` `pi` `e`.  
No spaces inside expressions.

Rendered as a textured triangle strip (billboard quads oriented toward camera).

```
curve helix   cos(t)  sin(t)  t/6  0 1 1
curve circle  cos(t)  sin(t)  0    1 0 0
curve sine    t/(2*pi)*4  sin(t)  0  0.2 0.9 0.3  -12.57 12.57
curve sketch  cos(t)  sin(t)  0  1 1 1  -6.28 6.28  pencil_thin
```

---

## Scene Commands

### BACKGROUND

```
background <texture>
```

Sets a fullscreen background texture for the scene. Rendered in screen-space (does not move with camera). Texture path is relative to `assets/textures/`.

```
background backgrounds/bg_paper.png
```

---

## Animations

Animation lines must appear **after** all entity declarations.

### ANIMATE

```
animate <entity_id> <property> <easing> <interpolation> <args...> <duration>
```

| Token | Values |
|---|---|
| `<property>` | `position` \| `rotation` \| `scale` |
| `<easing>` | `linear` \| `ease_in` \| `ease_out` \| `ease_in_out` |
| `<interpolation>` | `linear` (straight segments) \| `smooth` (Catmull-Rom spline) \| `path` (parametric expressions) |
| `<args>` | depends on interpolation mode (see below) |
| `<duration>` | total duration in seconds (last number on the line) |

**Key behavior:**
- The **start value is NOT in the file**. It's captured from the entity's current transform when the animation begins.
- The **easing** applies globally to `t / duration` — it controls acceleration over the entire animation, not per-segment.
- The **interpolation** defines the path shape between all points (start + waypoints).

#### Interpolation: `linear` / `smooth`

Args = one or more `x y z` waypoint triplets.

```
# Simple A→B: entity moves from current position to (3, 0, 0)
animate box  position  ease_out  linear   3 0 0   2.0

# Curved path: current pos → (3,2,0) → (3,5,0) → (0,5,0)
animate box  position  linear  smooth   3 2 0   3 5 0   0 5 0   3.0

# Rotation: current rotation → 180° on Y
animate box  rotation  ease_in_out  linear   0 180 0   1.5
```

#### Interpolation: `path`

Args = three parametric expressions `x(t)` `y(t)` `z(t)`.

```
animate <entity_id> <property> <easing> path <x_expr> <y_expr> <z_expr> <duration>
```

- `t` ranges from 0 to 1 (normalized progress, **after** easing is applied).
- The result is a **displacement** relative to `capturedStart`: final position = `capturedStart + (x(t), y(t), z(t))`.
- At `t=0` the expressions should return `(0,0,0)` to start at the current position.
- No spaces inside expressions. Same functions as curves: `sin`, `cos`, `tan`, `sqrt`, `abs`, `log`, `exp`, `pow`, `pi`, `e`.

```
# Spiral in XY over 3 seconds
animate my_quad position ease_in_out path cos(t*6.28)*2 sin(t*6.28)*2 t*5 3.0

# Smooth Y rotation
animate my_quad rotation linear path 0 t*360 0 2.0

# Parabolic bounce in Y
animate ball position ease_out path t*10 4*t*(1-t)*3 0 1.5
```

---

### WAIT

```
wait <seconds>
```

Offsets the `startTime` of every subsequent `animate` by `<seconds>`.  
Animations before and after a `wait` run in sequence. Animations with no `wait` between them start simultaneously.

```
animate box  position  ease_out  linear   3 0 0   2.0
wait 2.0
animate box  rotation  linear    linear   0 90 0  1.0
```

Timeline:

```
t=0 ──────────────── t=2
[position: current→3 ]

                      t=2 ──── t=3
                      [rotation: current→90°]
```

---

## Notes

- All scenes in `assets/scenes/` are loaded at startup.
- The first scene found becomes the active scene.
- Switch scenes at runtime with `ScenesManager::setCurrentScene(id)`.
- Scene id = filename without extension (`main_scene.txt` → `"main_scene"`).
- Animations chain naturally: if animation A moves box to (3,0,0) and animation B starts after, B captures (3,0,0) as its start.
