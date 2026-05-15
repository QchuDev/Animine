# Style System — References

## Técnica base: Textured Line Rendering

El approach es reemplazar primitivas GL por quads texturados. No es post-processing.

### Line rendering con quads

- [Thick and smooth 3D lines in OpenGL](https://vicrucann.github.io/tutorials/osg-shader-3dlines/) — quad generation, billboard orientation, miter joins
- [GPU-friendly Stroke Expansion](https://arxiv.org/html/2405.00127v1) — stroke rendering theory, joins and caps

### 9-slice / 3-slice

- [9-slice scaling (UI)](https://en.wikipedia.org/wiki/9-slice_scaling) — concepto original, adaptado a 1D para segmentos
- La variante 3-slice es simplemente el eje horizontal: cap-left, tileable-middle, cap-right

### Textured strokes en NPR

- [Real-Time Hatching (Praun et al.)](http://hhoppe.com/hatching.pdf) — TAMs, idea de texturas de trazo por intensidad
- [Rendering Rich Line Drawings](https://www.researchgate.net/publication/2409906_Surfaces_To_Lines_Rendering_Rich_Line_Drawings) — line-oriented NPR rendering

### OpenGL specifics

- [OpenGL Render-to-Texture](http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/) — si eventualmente se necesita un pass extra
- [The Book of Shaders](https://thebookofshaders.com/) — referencia general GLSL

### Inspiración visual

- [Moebius-style post-processing](https://blog.maximeheckel.com/posts/moebius-style-post-processing/) — estética hand-drawn en 3D (referencia visual, no técnica a usar)
- [Sable (game)](https://www.ign.com/games/sable) — líneas hand-drawn sobre 3D
