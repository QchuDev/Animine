# Animation System

Animations are declared inline in scene files, after entity definitions. They execute on a shared timeline controlled by `wait` offsets.

---

## Commands

### `animate`

Animate a property over a duration.

```
animate <id> <property> <easing> <interpolation> <args> <duration>
```

### `set`

Instantly set a property at the current timeline position.

```
set <id> <property> <x y z>
```

### `wait`

Advance the timeline cursor. All subsequent `animate`/`set` commands start after this offset.

```
wait <seconds>
```

---

## Properties

| Property | Values | Description |
|----------|--------|-------------|
| `position` | `x y z` | World position |
| `rotation` | `x y z` | Euler angles (degrees) |
| `scale` | `x y z` | Scale per axis |
| `color` | `r g b` | RGB color (0–1) |

---

## Easing functions

| Name | Curve |
|------|-------|
| `linear` | Constant speed |
| `ease_in` | Starts slow, accelerates |
| `ease_out` | Starts fast, decelerates |
| `ease_in_out` | Slow at both ends |

---

## Interpolation modes

### `linear`

Linearly interpolate between waypoints.

```
animate <id> <prop> <easing> linear <x y z> [<x y z> ...] <duration>
```

```
animate box position ease_out linear 0 0 0  3 2 0  5.0
animate box position ease_in_out linear 0 0 0  2 0 0  0 2 0  0 0 0  4.0
```

### `smooth`

Smooth (Catmull-Rom style) interpolation between waypoints. Same syntax as `linear`.

```
animate ball position ease_out smooth 0 0 0  1 2 0  3 0 0  3.0
```

### `path`

Parametric path defined by expressions in `t` (where `t` goes from 0 to 1).

```
animate <id> <prop> <easing> path <x(t)> <y(t)> <z(t)> <duration>
```

```
animate dot position linear path cos(t*6.28) sin(t*6.28) 0 3.0
animate obj scale ease_out path t t t 1.5
```

---

## Timeline flow

Commands execute in declaration order. `wait` accumulates:

```
# t=0: quad appears at origin
set my_quad position 0 0 0

# t=0: starts moving (takes 2s)
animate my_quad position ease_out linear 0 0 0  3 0 0  2.0

# t=2: wait shifts the cursor
wait 2.0

# t=2: starts rotating (takes 3s)
animate my_quad rotation linear linear 0 0 0  0 0 360  3.0

# t=3: another wait
wait 1.0

# t=3: color change
animate my_quad color ease_in linear 1 1 1  1 0 0  1.0
```

---

## Examples

**Scale in from zero:**
```
set obj scale 0 0 0
animate obj scale ease_out path t t t 1.0
```

**Orbit in a circle:**
```
animate dot position linear path cos(t*6.28)*2 0 sin(t*6.28)*2 4.0
```

**Pulse effect:**
```
animate obj scale ease_in_out linear 1 1 1  1.3 1.3 1.3  1 1 1  1.0
```

**Color fade:**
```
animate line1 color ease_in linear 1 1 1  0.2 0.6 1.0  2.0
```
