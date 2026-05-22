# Plan de Implementación

Ordenado de menor a mayor complejidad.

---

## Fase 1 — Fix Z-Fighting

**Problema:** Entidades coplanares parpadean porque el depth buffer no distingue cuál va adelante.

**Solución:** Usar el orden de declaración en el `.txt` como prioridad de profundidad.

**Pasos:**

1. Agregar `int creationOrder = 0` en `IEntity`
2. En `Scene::addEntity()`, asignar índice incremental al agregar cada entidad
3. En `Renderer::drawScene()`, usar `creationOrder` como desempate en el sort: a igual distancia, la entidad declarada después se dibuja encima
4. Aplicar `glPolygonOffset(-1.0 * index, -1.0)` antes de cada draw call (o un bias en clip-space via uniform)

**Archivos:**

| Archivo | Cambio |
|---------|--------|
| `include/classes/entities/entity.h` | Campo `creationOrder` |
| `src/classes/scenes/scene.cpp` | Asignar índice al agregar |
| `src/classes/render/renderer.cpp` | Desempate en sort + polygon offset |

---

## Fase 2 — Animate Color ✅

**Problema:** Solo se animan position, rotation y scale. El color es estático.

**Solución:** Agregar `COLOR` como propiedad animable. Las entidades ya tienen color internamente.

**Pasos:**

1. Agregar `COLOR` al enum `TransformProp`
2. Agregar `virtual glm::vec3 getColor()` / `virtual void setColor(glm::vec3)` en `IEntity`
3. Implementar en Line, Curve (ya tienen `glm::vec3 color`) y Quad (agregar tint uniform)
4. En `Animator::update()`, agregar case `COLOR` en captura y escritura
5. En `ScenesParser`, parsear `"color"` como propiedad válida en `animate` y `set`

**Sintaxis:**

```
animate my_line color ease_out linear 1 0 0 2.0
set my_line color 0 1 0
```

**Archivos:**

| Archivo | Cambio |
|---------|--------|
| `include/classes/animations/track.h` | `COLOR` en enum |
| `include/classes/entities/entity.h` | getColor/setColor virtuales |
| `src/classes/entities/line.cpp` | Implementar setColor (actualiza miembro) |
| `src/classes/entities/curve.cpp` | Implementar setColor |
| `src/classes/entities/quad.cpp` | Agregar tint uniform al draw |
| `src/classes/animations/animator.cpp` | Case COLOR en switches |
| `src/classes/creation/scenes_parser.cpp` | Parsear "color" |
| `shaders/quad_fragment.glsl` | Multiplicar por tint uniform |

---

## Fase 3 — Hot Reload

**Problema:** Hay que reiniciar el programa para ver cambios en los archivos de escena.

**Solución:** Polling de timestamps en el main loop. Si un archivo cambió, re-parsear y reemplazar la escena.

**Pasos:**

1. Guardar `std::map<string, fs::file_time_type>` con el timestamp de cada `.txt` al cargar
2. Cada ~60 frames, comparar `fs::last_write_time` contra el guardado
3. Si cambió: re-parsear con `ScenesParser::parseFile()`, reemplazar en `ScenesManager`
4. Si la escena activa es la modificada, resetear el Animator
5. Manejar errores de parseo sin crashear (ignorar archivos con errores temporales)

**Archivos:**

| Archivo | Cambio |
|---------|--------|
| `include/classes/engine.h` | Campo `fileTimestamps`, método `checkHotReload()` |
| `src/classes/engine.cpp` | Lógica de polling en `run()`, llamar a checkHotReload |
| `include/classes/scenes/scenes_manager.h` | Método `replaceScene(id, unique_ptr<Scene>)` |
| `src/classes/scenes/scenes_manager.cpp` | Implementar replaceScene |

**Notas:**
- Las texturas cacheadas no se limpian (se reusan)
- Los VAOs/VBOs de entidades viejas se liberan en sus destructores
- El parser necesita acceso al path original de cada escena (guardar en Scene o en el mapa)

---

## Fase 4 — Preset Loading / Groups ✅

**Problema:** No hay forma de agrupar entidades y animar el grupo como unidad.

**Solución:** Nuevo concepto de `Group` con transform propio. Los hijos heredan la transformación del padre al momento del draw.

**Pasos:**

1. Crear clase `Group : public IEntity` — no dibuja nada, solo tiene transform y lista de child IDs
2. Agregar campo `std::string parentId` en `IEntity` (vacío = sin padre)
3. En `Renderer::drawScene()`, antes de dibujar cada entidad, si tiene padre, multiplicar `parentModel * childModel`
4. Nuevo comando en parser: `group <id> <child1> <child2> ...`
5. (Opcional) Comando `preset <file>` que incluye otro .txt como sub-escena dentro de un grupo

**Sintaxis:**

```
line  a  0 0 0  1 0 0  1 0 0
line  b  0 0 0  0 1 0  0 1 0
group my_group  a b

animate my_group position ease_out linear 3 0 0 2.0
```

**Archivos:**

| Archivo | Cambio |
|---------|--------|
| Nuevo `include/classes/entities/group.h` | Clase Group |
| `include/classes/entities/entity.h` | Campo `parentId` |
| `src/classes/render/renderer.cpp` | Resolver jerarquía de transforms antes del draw |
| `src/classes/creation/scenes_parser.cpp` | Parsear `group` (y opcionalmente `preset`) |

**Decisión de diseño:** El grupo modifica la model matrix solo en el draw (no toca el transform local del hijo). Así las animaciones individuales sobre hijos siguen funcionando normalmente.

---

## Fase 5 — Meshes Support ✅

**Problema:** Solo hay líneas, quads y curvas. No se pueden cargar modelos 3D.

**Solución:** Nueva entidad `Mesh` que carga archivos `.obj` con tinyobjloader.

**Pasos:**

1. Integrar `tinyobjloader` (header-only) en `include/external/`
2. Crear clase `Mesh : public IEntity`:
   - Constructor: carga .obj, genera VAO/VBO (positions + normals + UVs)
   - `draw()`: usa un shader con iluminación diffuse básica
   - Soporta textura opcional
3. Nuevo shader `mesh_vertex.glsl` / `mesh_fragment.glsl` con 1 directional light
4. Parsear en ScenesParser: `mesh <id> <file.obj> [texture] [scale]`
5. Los archivos .obj van en `assets/meshes/`

**Sintaxis:**

```
mesh monkey  suzanne.obj  stone.png  0.5
mesh cube    cube.obj
```

**Archivos:**

| Archivo | Cambio |
|---------|--------|
| Nuevo `include/external/tiny_obj_loader.h` | Dependencia header-only |
| Nuevo `include/classes/entities/mesh.h` | Clase Mesh |
| Nuevo `src/classes/entities/mesh.cpp` | Carga .obj, genera buffers, draw |
| Nuevo `shaders/mesh_vertex.glsl` | MVP + normal transform |
| Nuevo `shaders/mesh_fragment.glsl` | Diffuse lighting + textura |
| `include/classes/render/renderer.h` | Puntero a meshShader |
| `src/classes/render/renderer.cpp` | Crear meshShader |
| `src/classes/creation/scenes_parser.cpp` | Parsear `mesh` |
| Nueva carpeta `assets/meshes/` | Archivos .obj |

**Notas:**
- Sin lighting los meshes se ven planos. Un diffuse con 1 luz direccional es mínimo y da volumen
- Solo triángulos, sin materiales multi-textura
- Transform y animaciones funcionan igual que cualquier entidad
