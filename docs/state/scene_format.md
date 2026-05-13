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
line <id> <x1> <y1> <z1> <x2> <y2> <z2> <r> <g> <b>
```

Draws a straight line from point 1 to point 2. Color is RGB in [0.0, 1.0].

```
line axis_x  0 0 0  1 0 0  1 0 0
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
curve <id> <x(t)> <y(t)> <z(t)> <r> <g> <b>
```

Parametric curve. `t` runs from `0` to `2π`. Expressions evaluated by tinyexpr.  
Supported: `+` `-` `*` `/` `^` `sin` `cos` `tan` `sqrt` `abs` `log` `exp` `pi` `e`.  
No spaces inside expressions.

```
curve helix   cos(t)  sin(t)  t/6  0 1 1
curve circle  cos(t)  sin(t)  0    1 0 0
```

---

## Animations

Animation lines must appear **after** all entity declarations.

### ANIMATE

```
animate <entity_id> <property> <easing> <interpolation> <x y z>... <duration>
```

| Token | Values |
|---|---|
| `<property>` | `position` \| `rotation` \| `scale` |
| `<easing>` | `linear` \| `ease_in` \| `ease_out` \| `ease_in_out` |
| `<interpolation>` | `linear` \| `smooth` (Catmull-Rom spline) |
| `<x y z>...` | 2 or more space-separated triplets — each is a keyframe value |
| `<duration>` | total duration in seconds (last number on the line) |

Keyframes are distributed evenly over `[0, duration]`.

```
# A→B transition
animate box  position  ease_out  linear   0 0 0   3 0 0   2.0

# Multi-point curved path (Catmull-Rom)
animate box  position  linear  smooth   0 0 0   3 2 0   3 5 0   0 5 0   3.0

# Rotation
animate box  rotation  ease_in_out  linear   0 0 0   0 180 0   1.5
```

---

### WAIT

```
wait <seconds>
```

Offsets the `startTime` of every subsequent `animate` by `<seconds>`.  
Animations before and after a `wait` run in sequence. Animations with no `wait` between them start simultaneously.

```
animate box  position  ease_out  linear   0 0 0   3 0 0   2.0
wait 2.0
animate box  rotation  linear    linear   0 0 0   0 90 0  1.0
```

Timeline:

```
t=0 ──────────────── t=2
[position: 0→3      ]

                      t=2 ──── t=3
                      [rotation: 0→90°]
```

---

## Notes

- All scenes in `assets/scenes/` are loaded at startup.
- The first scene found becomes the active scene.
- Switch scenes at runtime with `ScenesManager::setCurrentScene(id)`.
- Scene id = filename without extension (`main_scene.txt` → `"main_scene"`).
