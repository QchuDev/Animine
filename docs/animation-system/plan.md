# Animator — Implementation Plan

## Status

| Phase | Description | State |
|---|---|---|
| 1 | Data types (EasingType, Keyframe, Track, Animation) | ✅ done |
| 2 | evaluate() + easing functions + Catmull-Rom | ✅ done |
| 3 | Animator::update() connected to main loop | ✅ done |
| 4 | ScenesParser: animate / wait keywords | ✅ done |
| 5 | Test scene + verification | ✅ done |
| 6 | Redesign: global easing + runtime start capture | ✅ done |

---

## Design Decisions

| Question | Decision |
|---|---|
| Sequential or parallel? | Parallel with `startTime`; `wait` is a parser-side offset |
| Same `.txt` or separate file? | Same `.txt`, after entity declarations |
| Easing types at launch | `linear`, `ease_in`, `ease_out`, `ease_in_out` |
| Interpolation modes | `linear` (lerp), `smooth` (Catmull-Rom spline) |
| Loop support | No (first implementation) |
| Who applies the result? | `Animator` calls `entity->setPosition/setRotation/setScale` directly |
| Start value | Captured at runtime from entity's current transform (not in file) |
| Easing scope | Applied ONCE globally to `t/duration`, not per-segment |

---

## Core Concept

The `animate` command means: **"go to these waypoints, in this way"**.

- The **start value** is never written in the file — it's the entity's current transform when the animation begins.
- The **easing** controls how the entity accelerates/decelerates over the **total duration** (one global alpha).
- The **interpolation** defines the **shape of the path** between points (linear segments or smooth Catmull-Rom curve).

```
animate box  position  ease_out  smooth   3 2 0   3 5 0   0 5 0   3.0
```

At runtime:
1. Capture `box.getPosition()` → e.g. `(0,0,0)` → this becomes the path start.
2. Full path = `[(0,0,0), (3,2,0), (3,5,0), (0,5,0)]`
3. Each frame: `alpha = easing(t / 3.0)` → sample spline at `alpha` → write to entity.

---

## Scene File Format

```
quad  my_quad  wood.png  2 1

# Move to (3,2,0) with deceleration
animate my_quad  position  ease_out  linear   3 2 0   1.5

# Curved path through multiple waypoints
animate my_quad  position  linear  smooth   3 2 0   3 5 0   0 5 0   3.0

wait 1.5
animate my_quad  rotation  linear  linear   0 90 0  1.0
```

Syntax: `animate <entity_id> <property> <easing> <interpolation> <waypoints x y z>... <duration>`
- `<property>`: `position` | `rotation` | `scale`
- `<easing>`: `linear` | `ease_in` | `ease_out` | `ease_in_out`
- `<interpolation>`: `linear` | `smooth`
- Followed by 1+ `x y z` triplets (destination waypoints), then `duration`
- `wait <seconds>` — shifts `startTime` of all subsequent animations

---

## Data Types

```cpp
enum class EasingType        { LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT };
enum class TransformProp     { POSITION, ROTATION, SCALE };
enum class InterpolationMode { LINEAR, SMOOTH };

struct Keyframe { float time; glm::vec3 value; };  // time unused, kept for compat

struct Track {
    std::string       entity_id;
    TransformProp     property;
    EasingType        easing;
    InterpolationMode interpolation = InterpolationMode::LINEAR;
    std::vector<Keyframe> keyframes;   // waypoints only (destinations)

    glm::vec3 capturedStart{0.0f};     // filled at runtime
    bool      hasCaptured = false;
};

class Animation : public IAnimation {
public:
    float startTime = 0.0f;
    float duration  = 0.0f;
    std::vector<Track> tracks;
};
```

---

## Evaluation Pipeline

`evaluate(track, localT, duration)`:

1. Build full path: `[capturedStart, waypoint0, waypoint1, ...]`
2. Compute global alpha: `alpha = clamp(localT / duration, 0, 1)`
3. Apply easing: `alpha = applyEasing(alpha, track.easing)`
4. Map alpha to path segment: `seg = alpha * numSegments`
5. Dispatch: `LINEAR` → `glm::mix`, `SMOOTH` → Catmull-Rom with mirrored endpoints

---

## Animator::update()

```
for each active animation:
    localT = globalT - anim.startTime
    if localT not in [0, duration]: skip

    for each track:
        if not captured yet:
            track.capturedStart = entity->getProperty()
            track.hasCaptured = true
        value = evaluate(track, localT, duration)
        entity->setProperty(value)
```

---

## Files

| File | Role |
|---|---|
| `include/classes/animations/easing_type.h` | `EasingType` enum |
| `include/classes/animations/easing.h` | `applyEasing()` — 4 easing curves |
| `include/classes/animations/keyframe.h` | `Keyframe` struct |
| `include/classes/animations/track.h` | `Track` with capturedStart + hasCaptured |
| `include/classes/animations/animation.h` | `IAnimation` + `Animation` |
| `include/classes/animations/evaluate.h` | `evaluate()` + `catmullRom()` |
| `include/classes/animations/animator.h` | `Animator` class declaration |
| `src/classes/animations/animator.cpp` | `update()` implementation |
