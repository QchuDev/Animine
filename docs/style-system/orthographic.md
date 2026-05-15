# Cámara Ortográfica — Notas

## Comportamiento

- En **perspectiva**, objetos lejanos se ven más chicos. Las líneas billboard se achinan con la distancia.
- En **ortográfica**, todo se ve al mismo tamaño sin importar la profundidad. Las líneas mantienen grosor constante → mejor para estética hand-drawn.

## El salto (Tab)

Al togglear se cambia solo la matriz de proyección. La cámara (posición, orientación) no cambia. Esto significa:

- La escena "salta" visualmente porque la perspectiva desaparece de golpe
- La posición de la cámara sigue siendo la misma — si estabas lejos, en ortho vas a ver todo muy chico (necesitás ajustar zoom con Q/E)
- WASD sigue moviendo la cámara en world-space, funciona igual en ambos modos

## Zoom

En ortho el zoom no es "acercar la cámara" sino escalar los bounds del frustum:

```
orthoZoom = 5.0  →  ve [-5, 5] en Y (10 unidades de alto)
orthoZoom = 2.0  →  ve [-2, 2] en Y (4 unidades de alto, más "cerca")
```

Q agranda los bounds (aleja), E los achica (acerca). Mínimo clampeado a 0.5.

## Cosas a tener en cuenta

- **Sorting**: sigue funcionando por distancia a cámara, no hay problema
- **Billboard**: el cross product con la dirección a cámara sigue dando bien — las líneas se orientan correctamente
- **Depth buffer**: funciona igual, los objetos se tapan correctamente
- **Fondo**: no se ve afectado (ya es screen-space, ignora la proyección de la escena)
