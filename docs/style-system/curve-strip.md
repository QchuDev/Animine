# Curve Strip — 3-Slice UV System

## Stroke Texture Layout

Las texturas en `assets/textures/strokes/` siguen este layout horizontal:

```
u:  0        capU (0.25)    1-capU (0.75)      1
    ├──COLA────┼────MEDIO (tileable)────┼──CABEZA──┤
    v=1  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
         ░░╲░░░░═══════════════════════░░░░╱░░░░░░░
         ░░░╲░░░═══════════════════════░░╱░░░░░░░░░
    v=0  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
```

Requisitos:
- Fondo transparente (alpha = 0)
- La zona MEDIO debe ser seamless horizontalmente (tileable)
- La COLA es el inicio visual del trazo (punta izquierda)
- La CABEZA es el final visual del trazo (punta derecha)
- Color blanco/gris — el tint se aplica en el shader
- Resolución sugerida: 256x64 o 512x64

## Cómo funciona el Triangle Strip

Una curva se evalúa en ~200 puntos. Para cada punto se generan 2 vértices (±width perpendicular a la tangente, orientados a cámara como billboard). Esto forma un `GL_TRIANGLE_STRIP`.

```
v=1  ●───●───●───●───●───●───●───●
     |  /|  /|  /|  /|  /|  /|  /|
     | / | / | / | / | / | / | / |
     |/  |/  |/  |/  |/  |/  |/  |
v=0  ●───●───●───●───●───●───●───●
     u→  [cola][  tile 1  ][  tile 2  ][cabeza]
```

## Asignación de UVs (3-slice con caps fijos)

Los caps tienen tamaño fijo en unidades de mundo (`capWorldSize`), no proporcional al largo de la curva. Solo la zona intermedia se repite.

### Parámetros

| Variable | Valor | Descripción |
|----------|-------|-------------|
| `capWorldSize` | 0.25 | Unidades de arco que ocupa cada cap |
| `capU` | 0.25 | Fracción de la textura que ocupa cada cap en UV |
| `tileWorldSize` | 1.0 | Unidades de arco por repetición del tile intermedio |

### Zonas del arco

```
arcLen:  0       capWorldSize     totalLen - capWorldSize     totalLen
         ├──COLA──┼──────MEDIO (tiles)──────┼──CABEZA──┤
```

- **Cola** (`arcLen ∈ [0, capWorldSize]`): U va de `0` a `capU`
- **Medio** (`arcLen ∈ [capWorldSize, totalLen - capWorldSize]`): U va de `capU` a `1-capU` por cada `tileWorldSize` unidades de arco
- **Cabeza** (`arcLen ∈ [totalLen - capWorldSize, totalLen]`): U va de `1-capU` a `1`

## Cortes de tile (vértices duplicados)

Cuando la zona intermedia se repite, hay un salto de UV (`1-capU` → `capU`). Si se interpola linealmente entre esos dos valores, el GPU dibuja toda la textura comprimida en un solo quad.

Solución: en cada repetición del tile, se insertan **dos pares de vértices en la misma posición**:

```
... ●───●───●●───●───●● ...
         tile 1  ↑  tile 2  ↑
              close/open  close/open
```

1. Par con `u = 1-capU` → cierra el tile anterior
2. Par con `u = capU` → abre el tile siguiente

El quad degenerado entre ambos (misma posición, distinto UV) tiene área cero y no se ve.

## Miter Join

En curvas con curvatura, los quads adyacentes no comparten el mismo borde porque el vector lateral cambia de dirección. Esto deja gaps triangulares en el interior de la curva.

Solución: en cada vértice interior, se escala el ancho por `1/cos(halfAngle)` entre los segmentos adyacentes:

```cpp
glm::vec3 d0 = normalize(points[i] - points[i-1]);
glm::vec3 d1 = normalize(points[i+1] - points[i]);
float cosHalf = length((d0 + d1) * 0.5f);
float miter = 1.0f / cosHalf;
miter = min(miter, 2.0f);  // clamp para ángulos extremos
```

Esto ensancha el strip en las curvas para que los bordes se encuentren sin gap.
