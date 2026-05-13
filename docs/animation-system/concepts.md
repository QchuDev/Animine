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
