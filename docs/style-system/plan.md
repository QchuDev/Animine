# Style System — Plan

## Filosofía

Todo el estilo visual se basa en texturas dibujadas a mano. No hay generación procedural ni post-processing. El engine toma los assets tal cual y los renderiza con la estética que vos definís en los PNGs.

---

## 1. Líneas y Curvas — 3-Slice Texture

### Concepto

Reemplazar el `GL_LINES` actual por quads texturados que usan una textura hand-drawn con 3 regiones:

```
┌───────┬──────────────────────────┬───────┐
│ START │     MIDDLE (tileable)    │  END  │
│ (cap) │     se repite en X       │ (cap) │
└───────┴──────────────────────────┴───────┘
```

- **Start cap**: punta inicial del trazo (ej: presión del lápiz al apoyar)
- **Middle**: sección que se repite (tile) según la longitud del segmento
- **End cap**: punta final (ej: el trazo levantándose del papel)

### Cómo funciona

1. Cada segmento de línea se convierte en un **quad billboard** orientado entre los dos puntos, con un ancho configurable (grosor del trazo)
2. Las UVs se calculan para que:
   - `u = 0..capSize` → región start
   - `u = capSize..1-capSize` → región middle (tileada según longitud)
   - `u = 1-capSize..1` → región end
   - `v = 0..1` → ancho del trazo
3. El fragment shader samplea la textura y aplica el color de la línea como tint (`texture.rgb * lineColor`)

### Para curvas

Mismo sistema pero aplicado a cada segmento consecutivo de la curva (polyline de N puntos).

**Optimizaciones:**

- **Single VBO**: todos los quads de la curva se generan en un solo buffer. Un único draw call para toda la curva (como ya se hace con `GL_LINE_STRIP`).
- **Triangle strip**: en vez de 6 verts/segmento (quads independientes), se usa `GL_TRIANGLE_STRIP` donde cada segmento nuevo agrega solo 2 vértices (los del lado B). Resultado: N segmentos = 2N+2 vértices, 1 draw call.
- Los caps (start/end) solo se aplican al primer y último segmento de la curva.

### Texturas necesarias

| Asset | Descripción |
|-------|-------------|
| `line_default.png` | Trazo estándar de lápiz/marcador |
| (futuro) variantes | Diferentes estilos de trazo |

---

## 2. Entorno / Fondo

### Concepto

El fondo es un quad fijo en screen-space que no se mueve con la cámara. Se dibuja con proyección ortográfica ignorando la view matrix, como un wallpaper detrás de todo.

### Implementación

- Se renderiza **primero**, antes de cualquier entidad
- Usa una projection ortográfica propia (no la de la cámara)
- Sin depth write (`glDepthMask(GL_FALSE)`) para que no interfiera con la escena
- La textura se estira al viewport completo (o se tilea si se prefiere)

### Capas

| Capa | Cuándo se dibuja | Descripción |
|------|------------------|-------------|
| Fondo (papel) | Primero, antes de todo | Textura fija en screen-space, sin view matrix |
| Ejes estilizados | Con las entidades | Usan el sistema 3-slice como cualquier línea |
| Viñeta/marco | Último, sin depth test | (Opcional) Overlay decorativo en screen-space |

### Texturas necesarias

| Asset | Descripción |
|-------|-------------|
| `bg_paper.png` | Textura de fondo (papel/cuaderno) |
| `vignette.png` | (opcional) Bordes/marco decorativo |

---

## 3. Implementación — Pasos

### Fase 1: Line rendering con texturas (core) ✓

1. ~~Crear clase `TexturedLine` (o modificar `Line`) que genera un quad entre dos puntos~~
2. ~~Calcular orientación del quad (billboard hacia cámara)~~
3. ~~Implementar UV mapping con 3-slice logic~~
4. ~~Fragment shader: `texture * tintColor`, con alpha test para bordes del trazo~~

### Fase 2: Curvas optimizadas ✓

5. ~~Generar todos los quads de la curva en un solo VBO como `GL_TRIANGLE_STRIP`~~
6. ~~Cada segmento agrega 2 vértices (lado B), caps solo en extremos~~
7. ~~UV mapping: caps en primer/último segmento, middle tileado en el resto~~

### Fase 3: Fondo fijo ✓

8. ~~Quad fullscreen en screen-space con projection ortográfica, sin view matrix~~
9. ~~Se dibuja primero, sin depth write~~
10. ~~Comando `background <texture>` en el parser~~

### Fase 4: Ejes como Lines ✓

11. ~~Reemplazar clase `Axises` por 3 instancias de `Line` creadas en Renderer~~
12. ~~Ejes con longitud [-0.2, 0.2], colores RGB, misma textura de trazo~~

### Fase 5: Cámara ortográfica

15. Toggle perspectiva/ortográfica (1 línea en `Renderer::drawScene`)
16. Agregar `zoom` float a la cámara para escalar bounds del ortho
17. Ajustar input de zoom (scroll o teclas) para modificar los bounds en vez de mover la cámara

**Consideraciones:**

| Aspecto | Perspectiva | Ortográfica |
|---------|-------------|-------------|
| Cámara (WASD) | Funciona igual | Funciona igual |
| Sorting back-to-front | Por distancia | Sigue funcionando |
| Billboard de líneas | Mismo cálculo | Mismo cálculo |
| Grosor de líneas | Se achica con distancia | Constante en pantalla (mejor para hand-drawn) |
| Zoom | Mover cámara | Escalar bounds del ortho |

---

## 4. Cambios al engine

| Componente | Cambio |
|------------|--------|
| `Line` | Pasa de `GL_LINES` a quad texturado (billboard) |
| `Curve` | Single VBO + `GL_TRIANGLE_STRIP`, caps solo en extremos |
| `Renderer` | Dibuja fondo fijo (ortho, sin view) antes de la escena |
| `Shaders` | Nuevo `stroke_vertex.glsl` / `stroke_fragment.glsl` (tint + alpha) |
| Scene format | Nuevo comando `background <texture>` |

---

## 5. Estructura de assets

```
assets/
  textures/
    strokes/
      line_default.png    ← trazo principal
    backgrounds/
      bg_paper.png        ← fondo
    overlays/
      vignette.png        ← marco (opcional)
```
