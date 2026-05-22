# Groups & Presets

Organize entities into hierarchical units for collective animation and reuse.

---

## Groups

Group existing entities to animate them as a single unit. Children inherit the group's transform while keeping their own local transforms.

```
group <id> <child1> <child2> ...
```

| Parameter | Description |
|-----------|-------------|
| `id` | Unique identifier for the group |
| `children` | Space-separated list of existing entity IDs |

### Rules

- Children must be declared **before** the `group` command
- Animating `position`, `rotation`, `scale` on a group moves all children together
- Animating `color` on a group propagates to every child
- Children can still be animated individually (local transforms stack with the group's)
- Groups don't render anything — they only provide a parent transform

### Example

```
line x_axis  0 0 0  2 0 0  1 0.3 0.3
line y_axis  0 0 0  0 2 0  0.3 1 0.3
line z_axis  0 0 0  0 0 2  0.3 0.3 1

group axes  x_axis y_axis z_axis

# Rotate all three axes together
animate axes rotation ease_in_out linear 0 0 0  0 360 0  4.0

# Change all axes to white
animate axes color linear linear 1 1 1 2.0

# Still animate one child individually
animate x_axis scale ease_out linear 1 1 1  2 1 1  1.0
```

---

## Presets

Reusable sub-scenes loaded from `assets/presets/`. A preset file contains entity declarations that get instantiated as a group with prefixed IDs.

```
preset <id> <file.txt>
```

| Parameter | Description |
|-----------|-------------|
| `id` | Name for the resulting group |
| `file.txt` | File in `assets/presets/` containing entity declarations |

### How it works

1. The engine reads the preset file
2. Each entity's ID is prefixed with `<preset_id>.` to avoid collisions
3. All entities are added to the scene under a new group
4. The group can be animated like any other

### Preset file format

Only entity declarations (`line`, `quad`, `curve`). No animations — those belong in the scene.

```
# assets/presets/axes.txt
line x  0 0 0  1 0 0  1 0.3 0.3
line y  0 0 0  0 1 0  0.3 1 0.3
line z  0 0 0  0 0 1  0.3 0.3 1
```

### Usage in a scene

```
preset my_axes  axes.txt
preset other_axes  axes.txt

set my_axes position -2 0 0
set other_axes position 2 0 0

animate my_axes rotation ease_out linear 0 0 0  0 360 0  3.0
```

### Accessing children

Children are accessible with dot notation: `<preset_id>.<child_id>`

```
preset my_axes  axes.txt

# Animate just the x-axis line within the preset
animate my_axes.x color ease_in linear 1 0.3 0.3  1 1 0  1.0
```

---

## Nesting

Groups cannot currently contain other groups. Keep hierarchies flat — one level of grouping is sufficient for most visualizations.

---

## File organization

```
assets/
  presets/
    axes.txt        # reusable coordinate axes
    grid.txt        # reusable grid
    unit_circle.txt # reusable circle
  scenes/
    my_scene.txt    # uses presets and groups
```
