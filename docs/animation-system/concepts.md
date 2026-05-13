# Animation System — Concepts

## Keyframes + Interpolation

A **keyframe** is a `(time, value)` pair. The animator answers one question every frame:
> *"Given that I'm at time `t`, what is the value of this property?"*

```
t=0.0  →  position = (0, 0, 0)
t=1.0  →  position = (3, 2, 0)
t=0.4  →  position = lerp((0,0,0), (3,2,0), 0.4) = (1.2, 0.8, 0)
```

The animatable properties in QchuAnims map directly to `Transform`: `position`, `rotation`, `scale`.

---

## Easing Functions

Linear interpolation looks mechanical. Easing functions modify the **alpha** before passing it to lerp:

```cpp
// linear
float alpha = t / duration;
value = lerp(from, to, alpha);

// ease out (decelerates on arrival)
float alpha = 1.0f - pow(1.0f - t / duration, 2.0f);
value = lerp(from, to, alpha);

// ease in (accelerates from start)
float alpha = pow(t / duration, 2.0f);
value = lerp(from, to, alpha);
```

Visual reference: https://easings.net

Minimum viable set for a first implementation: `linear`, `ease_in`, `ease_out`, `ease_in_out`.

---

## How an Animator Works

The animator holds a global time `t` that advances by `deltaTime` each frame.
For each active animation:

1. Find the two keyframes surrounding the current `t`.
2. Compute the normalized alpha between them.
3. Apply the easing function.
4. Write the result to the entity's transform.

```
Animator::update(deltaTime):
    t += deltaTime
    for each active animation:
        find keyframe_before, keyframe_after  where  keyframe_before.t <= t <= keyframe_after.t
        alpha = (t - keyframe_before.t) / (keyframe_after.t - keyframe_before.t)
        alpha = applyEasing(alpha, easing_type)
        value = lerp(keyframe_before.value, keyframe_after.value, alpha)
        entity->setProperty(property, value)
```

---

## Sequential vs Parallel

The most important design decision before implementing:

| Mode | Description | `wait` makes sense? |
|------|-------------|---------------------|
| **Sequential** | Animations run one after another, driven by a cursor | Yes |
| **Parallel** | Each animation has its own `start_time`, all run simultaneously | No — use `start_time` offset instead |
| **Mixed** | Parallel tracks, `wait` sequences within a track | Yes |

Most engines use **parallel tracks** — each property of each entity has its own timeline.
`wait` is syntactic sugar that offsets the `start_time` of the next command.

---

## Design Questions to Answer Before Implementing

1. **Sequential or parallel?** (or mixed with `wait` to sequence)
2. **Same `.txt` as entities, or separate animation files?**
3. **Which easing types at launch?** (`linear`, `ease_in`, `ease_out` is enough to start)
4. **Loop support?** Can an animation repeat?
5. **Who applies the result?** Animator calls `entity->setPosition()` directly, or via a message/event system?

---

## Path Animation

### The problem with linear lerp between keyframes

With `lerp(a, b, alpha)` between consecutive keyframes, the path is always a straight line segment. An object moving through 4 keyframes traces a **polyline** — it reaches each point exactly, but the direction changes abruptly at every keyframe. This looks mechanical for position tracks.

```
K0 ──────── K1
             \
              \──── K2 ──── K3
```

There are two families of solutions:

---

### Option A — Catmull-Rom Spline (recommended)

Catmull-Rom is a **cubic spline** that passes through every keyframe and automatically computes smooth tangents from the surrounding points. No extra data needed — just the keyframes you already have.

```
given four consecutive keyframes: K(i-1), K(i), K(i+1), K(i+2)
alpha ∈ [0, 1]  (local t between K(i) and K(i+1))

p(alpha) = 0.5 * (
    (2 * K(i))
  + (-K(i-1) + K(i+1)) * alpha
  + (2*K(i-1) - 5*K(i) + 4*K(i+1) - K(i+2)) * alpha²
  + (-K(i-1) + 3*K(i) - 3*K(i+1) + K(i+2)) * alpha³
)
```

For the first and last segments, mirror the endpoint to synthesize the missing neighbor:
`K(-1) = 2*K(0) - K(1)`.

**Pros:** smooth C1 continuity, no extra authoring, works with the existing `vector<Keyframe>`.  
**Cons:** requires at least 4 keyframes to be meaningful; with only 2 it degrades to linear.

GLM has no built-in Catmull-Rom, but it's ~15 lines of code. See [^1].

---

### Option B — Bézier Curves (explicit control points)

A cubic Bézier uses 4 points: 2 anchors (the keyframe positions) and 2 control points (tangent handles). The path does **not** pass through the control points — they pull the curve.

```
B(alpha) = (1-α)³·P0 + 3(1-α)²α·P1 + 3(1-α)α²·P2 + α³·P3
```

This is what tools like Blender's graph editor and CSS `cubic-bezier()` use.

**Pros:** precise artistic control over the curve shape.  
**Cons:** requires authoring control points per segment — harder to define in a plain `.txt` file.

---

### Which to use in QchuAnims

| | Catmull-Rom | Bézier |
|---|---|---|
| Extra data in `.txt` | None | 2 control points per segment |
| Passes through keyframes | Yes | Yes (anchors only) |
| Smooth by default | Yes | Only if control points are set well |
| Implementation cost | Low | Medium |

**Recommendation:** implement Catmull-Rom as the `SMOOTH` interpolation mode on a `Track`. Keep `LINEAR` as the default. The scene file just needs one extra token:

```
animate box  position  smooth   0 0 0   3 2 0   3 5 0   0 5 0   duration 3.0
```

The `evaluate()` function checks the track's interpolation mode and dispatches to either `lerp` or `catmullRom`.

---

### Impact on current architecture

`Track` already holds `vector<Keyframe>` — no struct changes needed. The only addition is an `InterpolationMode` field on `Track` (or reuse `EasingType` with a `SMOOTH` variant). The change is isolated to `evaluate()`.

[^1]: Catmull-Rom implementation reference: *Smooth interpolation of irregularly spaced keyframes* — https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/smooth-interpolation-of-irregularly-spaced-keyframes-r1497
