# Animator — Implementation Plan

## Status

| Phase | Description | State |
|---|---|---|
| 1 | Data types (EasingType, Keyframe, Track, Animation) | ✅ done |
| 2 | evaluate() + easing functions + Catmull-Rom | ✅ done |
| 3 | Animator::update() connected to main loop | ✅ done |
| 4 | ScenesParser: animate / wait keywords | ✅ done |
| 5 | Test scene + verification | ⬜ next |

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

---

## Scene File Format

```
quad  my_quad  wood.png  2 1

# A→B transition
animate my_quad  position  ease_out  linear   0 0 0   3 2 0   1.5

# Multi-keyframe path (Catmull-Rom)
animate my_quad  position  linear  smooth   0 0 0   3 2 0   3 5 0   0 5 0   3.0

wait 1.5
animate my_quad  rotation  linear  linear   0 0 0   0 90 0  1.0
```

Syntax: `animate <entity_id> <property> <easing> <interpolation> <x y z>... <duration>`
- `<property>`: `position` | `rotation` | `scale`
- `<easing>`: `linear` | `ease_in` | `ease_out` | `ease_in_out`
- `<interpolation>`: `linear` | `smooth`
- Followed by 2+ `x y z` triplets (keyframe values), then `duration`
- `wait <seconds>` — shifts `startTime` of all subsequent animations

---

## Phase 1 — Data Types ✅

Files created:
- `include/classes/animations/easing_type.h` — `EasingType` enum
- `include/classes/animations/keyframe.h`    — `Keyframe { float time; glm::vec3 value; }`
- `include/classes/animations/track.h`       — `TransformProp`, `InterpolationMode`, `Track`
- `include/classes/animations/animation.h`   — `IAnimation` (virtual dtor) + `Animation` class

```cpp
enum class EasingType        { LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT };
enum class TransformProp     { POSITION, ROTATION, SCALE };
enum class InterpolationMode { LINEAR, SMOOTH };

struct Keyframe { float time; glm::vec3 value; };

struct Track {
    std::string       entity_id;
    TransformProp     property;
    EasingType        easing;
    InterpolationMode interpolation = InterpolationMode::LINEAR;
    std::vector<Keyframe> keyframes;
};

class Animation : public IAnimation {
public:
    float startTime = 0.0f;
    float duration  = 0.0f;
    std::vector<Track> tracks;
};
```

---

## Phase 2 — Evaluation ✅

Files created:
- `include/classes/animations/easing.h`   — `applyEasing(alpha, EasingType)` inline
- `include/classes/animations/evaluate.h` — `evaluate(track, localT) → glm::vec3`

`evaluate()` logic:
1. Clamp to first/last keyframe if out of range
2. Find segment `[kf[i], kf[i+1]]` containing `localT`
3. Compute `alpha`, apply easing
4. Dispatch: `LINEAR` → `glm::mix`, `SMOOTH` → Catmull-Rom

Catmull-Rom: mirrors endpoints to synthesize missing neighbors (`p(-1) = 2·p0 - p1`).

---

## Phase 3 — Animator::update() ✅

Files modified/created:
- `include/classes/animations/animator.h` — rewritten: `update(float, Scene*)`, `reset()`
- `src/classes/animations/animator.cpp`   — implementation
- `include/classes/scenes/scene.h`        — added `getEntity(id) → IEntity*`
- `src/classes/scenes/scene.cpp`          — implementation of `getEntity`
- `src/classes/engine.cpp`                — `animator = new Animator()` in `init()`, `animator->update(deltaTime, scene)` before `drawScene()`, `delete animator` in destructor

---

## Phase 4 — ScenesParser extension ✅

Files modified:
- `include/classes/creation/scenes_parser.h` — added `createAnimation(ss, startTime, scene)` declaration
- `src/classes/creation/scenes_parser.cpp`   — implemented `createAnimation()`; `parseFile()` now handles `animate`/`wait` with a local `timeOffset` accumulator; `parseLine`/`createEntity` unchanged

---

## Phase 5 — Test Scene

File: `assets/scenes/anim_test.txt`

```
quad  box  wood.png  1 1

animate box  position  ease_out  smooth   0 0 0   3 0 0   3 3 0   2.0
wait 2.0
animate box  rotation  ease_in_out  linear   0 0 0   0 180 0  1.5
```

Expected: box follows a curved path over 2 s, then spins 180° over 1.5 s.
