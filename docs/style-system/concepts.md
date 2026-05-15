# Style System — Concepts

## 3-Slice Line Rendering

### El problema

`GL_LINES` dibuja segmentos de 1px sin estilo. No hay forma de darles textura, grosor variable, ni aspecto hand-drawn.

### La solución

Convertir cada segmento en un **quad** (2 triángulos) con una textura de trazo dibujada a mano.

### Geometría

Dado un segmento de A a B:

```
        normal (perpendicular)
           ↑
    v1 ────────────────── v2
    │                      │
  A ●━━━━━━━━━━━━━━━━━━━━━● B    (dirección del segmento)
    │                      │
    v0 ────────────────── v3
```

- `dir = normalize(B - A)`
- `normal = perpendicular a dir` (en 2D: `vec2(-dir.y, dir.x)`, en 3D: cross con view direction)
- `v0 = A - normal * halfWidth`
- `v1 = A + normal * halfWidth`
- `v2 = B + normal * halfWidth`
- `v3 = B - normal * halfWidth`

### UV Mapping (3-slice)

La textura tiene 3 zonas horizontales:

```
u:  0          capU        1-capU         1
    ├──START────┼───MIDDLE (tile)───┼──END──┤
```

`capU` es la proporción de la textura que ocupa cada cap (ej: 0.15 = 15% cada punta).

Para un segmento de longitud `L` con un `tileScale` que define cuántas unidades de mundo cubre un tile del middle:

```
middleRepeats = (L - 2 * capWorldSize) / tileScale
```

UVs del quad:

| Vértice | u | v |
|---------|---|---|
| v0 (A, abajo) | 0 | 0 |
| v1 (A, arriba) | 0 | 1 |
| v2 (B, arriba) | capU + middleRepeats * (1 - 2*capU) + capU → simplificado | 1 |
| v3 (B, abajo) | (mismo que v2) | 0 |

En la práctica, el fragment shader hace el slicing:

```glsl
uniform float capU;       // ej: 0.15
uniform float tileCount;  // middleRepeats calculado en CPU

void main() {
    float u = texCoord.x;
    float mappedU;

    if (u < capU) {
        // Start cap — mapea [0, capU] → [0, capU] de la textura
        mappedU = u;
    } else if (u > 1.0 - capU) {
        // End cap — mapea [1-capU, 1] → [1-capU, 1] de la textura
        mappedU = u;
    } else {
        // Middle — tile entre [capU, 1-capU] de la textura
        float middleU = (u - capU) / (1.0 - 2.0 * capU); // normaliza a [0,1]
        float tiledU = fract(middleU * tileCount);         // repite
        mappedU = capU + tiledU * (1.0 - 2.0 * capU);     // remapea al rango middle
    }

    vec4 tex = texture2D(strokeTexture, vec2(mappedU, texCoord.y));
    gl_FragColor = tex * vec4(lineColor, 1.0);
}
```

### Orientación del quad

Dos opciones:

1. **Billboard** — el quad siempre mira a la cámara. Grosor constante en pantalla sin importar la distancia. Bueno para estética 2D.
2. **Flat** — el quad vive en un plano fijo (ej: XY). El grosor se ve afectado por perspectiva. Más "3D".

Para este proyecto, **billboard** tiene más sentido porque refuerza la estética de dibujo plano.

---

## Curvas como Polyline de Quads

Una curva ya se genera como N puntos evaluando la función paramétrica. Actualmente se dibuja con `GL_LINE_STRIP`.

Con el nuevo sistema:
- Para cada par de puntos consecutivos `(P[i], P[i+1])`, se genera un quad con el 3-slice
- Los caps solo se aplican al **primer** y **último** segmento de la curva
- Los segmentos intermedios usan solo la región middle
- En los joints entre segmentos, los vértices se comparten para evitar gaps

---

## Textura de Trazo — Requisitos

La textura PNG que dibujes debe cumplir:

1. **Horizontal** — el trazo va de izquierda a derecha
2. **Fondo transparente** (alpha = 0 donde no hay trazo)
3. **Tileable en el centro** — la zona middle debe poder repetirse sin corte visible
4. **Caps diferenciados** — las puntas tienen forma de inicio/fin de trazo
5. **Blanco o gris** — el color se aplica como tint en el shader (`tex * color`)

Ejemplo de layout:

```
┌─────────────────────────────────────────────┐
│ ╲    ═══════════════════════════════    ╱    │
│  ╲   ═══════════════════════════════  ╱     │  ← trazo con alpha
│   ╲  ═══════════════════════════════ ╱      │
└─────────────────────────────────────────────┘
  cap          middle (tileable)          cap
```

Resolución sugerida: 256x64 o 512x64 (ancho para detalle del tile, alto para el grosor del trazo).
