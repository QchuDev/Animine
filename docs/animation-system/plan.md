# Animator — Implementation Plan

## Design Decisions

| Question | Decision |
|---|---|
| Sequential or parallel? | Parallel with `startTime`; `wait` is a parser-side offset |
| Same `.txt` or separate file? | Same `.txt`, after entity declarations |
| Easing types at launch | `linear`, `ease_in`, `ease_out`, `ease_in_out` |
| Loop support | No (first implementation) |
| Who applies the result? | `Animator` calls `entity->setPosition/setRotation/setScale` directly |

---

## Scene File Format (extension)

```
quad  my_quad  wood.png  2 1

animate my_quad  position  ease_out   0 0 0   3 2 0   1.5
wait 1.5
animate my_quad  rotation  linear     0 0 0   0 90 0  1.0
```

- `animate <entity_id> <property> <easing> <x0 y0 z0> <x1 y1 z1> <duration>`
- `wait <seconds>` — offsets the `startTime` of every subsequent animation

---

## Phase 1 — Data Types

Files to create / modify:

- `include/classes/animations/easing.h`  — `EasingType` enum
- `include/classes/animations/keyframe.h` — `Keyframe` struct
- `include/classes/animations/track.h`    — `Track` struct (`entity_id`, `property`, `easing`, keyframes)
- `include/classes/animations/animation.h` — replace empty `IAnimation`; add concrete `Animation` class

**Key types:**

```cpp
enum class EasingType { LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT };

enum class TransformProp { POSITION, ROTATION, SCALE };

struct Keyframe { float time; glm::vec3 value; };

struct Track {
    std::string   entity_id;
    TransformProp property;
    EasingType    easing;
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

## Phase 2 — Evaluation

Files to create:

- `include/classes/animations/easing.h` — easing functions (inline)
- `include/classes/animations/evaluate.h` — `evaluate(track, localT) → glm::vec3`

**Logic:**

```
evaluate(track, localT):
    find keyframe_a, keyframe_b  where  a.time <= localT <= b.time
    alpha = (localT - a.time) / (b.time - a.time)
    alpha = applyEasing(alpha, track.easing)
    return lerp(a.value, b.value, alpha)
```

Easing formulas (alpha in [0,1]):

| Type | Formula |
|---|---|
| linear | `alpha` |
| ease_in | `alpha²` |
| ease_out | `1 - (1-alpha)²` |
| ease_in_out | `alpha < 0.5 ? 2α² : 1 - 2(1-α)²` |

---

## Phase 3 — Animator::update()

Files to modify:

- `include/classes/animations/animator.h`
- `src/classes/animations/animator.cpp`
- `src/classes/engine.cpp` — connect to main loop

**Signature:**

```cpp
void Animator::update(float deltaTime, Scene* scene);
```

**Logic:**

```
t += deltaTime
for each Animation* anim in scene->getAnimations():
    localT = t - anim->startTime
    if localT < 0 or localT > anim->duration: skip
    for each Track& track in anim->tracks:
        vec3 value = evaluate(track, localT)
        IEntity* e = scene->getEntity(track.entity_id)
        if property == POSITION: e->setPosition(value)
        if property == ROTATION: e->setRotation(value)
        if property == SCALE:    e->setScale(value)
```

---

## Phase 4 — ScenesParser extension

File to modify: `include/classes/creation/scenes_parser.h` / `.cpp`

- Add `float timeOffset = 0.0f` local to `parseFile()`
- On `animate` line → build a `Track` with two keyframes (t=0, t=duration), wrap in `Animation` with `startTime = timeOffset`, add to scene
- On `wait N` line → `timeOffset += N`

---

## Phase 5 — Test Scene & Verification

File: `assets/scenes/anim_test.txt`

```
quad  box  wood.png  1 1

animate box  position  ease_out   0 0 0   3 0 0   2.0
wait 2.0
animate box  rotation  ease_in_out  0 0 0   0 180 0  1.5
```

Expected: box slides right over 2 s, then spins 180° over 1.5 s.
