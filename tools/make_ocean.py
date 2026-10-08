#!/usr/bin/env python3
"""
Genera assets/scenes/ocean.txt — un "mar" de 100 curvas senoidales.

Modelo:
  - 50 curvas-fila corren a lo largo de X, cada una en un Z distinto.
  - 50 curvas-columna corren a lo largo de Z, cada una en un X distinto.
  Juntas forman una malla de 100 curvas que ondula.

Cada curva lleva horneada una leve onda espacial (su cresta estatica).
La ilusion de "mar" la da el BOBBING VERTICAL: animamos la posicion Y
de cada curva con interpolacion `path`, donde

    y(t) = AMP * sin( 2*pi*CICLOS*t + FASE )

y la FASE depende de la posicion (x + z) de la curva, de modo que las
crestas se propagan en diagonal como un oleaje real.

El motor hace clamp del tiempo a [0, DURACION] y mantiene el ultimo valor,
asi que usamos una DURACION larga con muchos CICLOS: el mar se mece de
forma fluida durante toda la reproduccion.
"""
import math

# --- Parametros del dominio -------------------------------------------------
N          = 50       # curvas por eje  (50 en X + 50 en Z = 100)
HALF       = 6.0      # el mar va de -HALF..+HALF en X y en Z
SPACING    = (2 * HALF) / (N - 1)

# --- Onda espacial horneada (la forma estatica de cada curva) ---------------
SPACE_AMP  = 0.35     # altura de la ondulacion espacial
SPACE_FREQ = 1.1      # cuantas crestas a lo largo de la curva

# --- Bobbing vertical animado ----------------------------------------------
BOB_AMP    = 0.9      # cuanto sube/baja cada curva
CYCLES     = 9        # oscilaciones completas durante la animacion
DURATION   = 48.0     # segundos de animacion
WAVELEN    = 7.0      # longitud de onda del oleaje (controla el desfase)

# --- Estetica ---------------------------------------------------------------
STROKE     = "line_default"
# color base azul mar; cada curva varia ligeramente el tono con la distancia
def color_for(dist_norm):
    # de azul profundo (centro) a cian claro (bordes)
    r = 0.10 + 0.10 * dist_norm
    g = 0.45 + 0.35 * dist_norm
    b = 0.75 + 0.25 * dist_norm
    return r, g, b

lines = []
lines.append("# ============================================================")
lines.append("#  ocean — mar de 100 ondas senoidales (GENERADO: make_ocean.py)")
lines.append("#  50 curvas en X + 50 en Z, bobbing vertical desfasado.")
lines.append("# ============================================================")
lines.append("")
lines.append("background backgrounds/paper.png")
lines.append("")

def phase_for(x, z):
    # desfase segun distancia radial proyectada -> oleaje diagonal
    return (x + z) / WAVELEN * 2.0 * math.pi

entities = []   # (id, x_expr, y_expr, z_expr, color, tmin, tmax)
anims    = []   # (id, y_expr_path)

# --- Curvas-fila: corren a lo largo de X, Z = const -------------------------
for i in range(N):
    z = -HALF + i * SPACING
    cid = f"rowx_{i}"
    # x = t ; y = onda espacial ; z = cte
    x_expr = "t"
    y_expr = f"{SPACE_AMP}*sin({SPACE_FREQ}*t)"
    z_expr = f"{z:.4f}"
    dist = abs(z) / HALF
    entities.append((cid, x_expr, y_expr, z_expr, color_for(dist), -HALF, HALF))
    ph = phase_for(0.0, z)
    y_path = f"{BOB_AMP}*sin(6.2832*{CYCLES}*t+{ph:.4f})"
    anims.append((cid, y_path))

# --- Curvas-columna: corren a lo largo de Z, X = const ----------------------
for j in range(N):
    x = -HALF + j * SPACING
    cid = f"colz_{j}"
    # parametrizamos con t = z ; x = cte ; y = onda espacial
    x_expr = f"{x:.4f}"
    y_expr = f"{SPACE_AMP}*sin({SPACE_FREQ}*t)"
    z_expr = "t"
    dist = abs(x) / HALF
    entities.append((cid, x_expr, y_expr, z_expr, color_for(dist), -HALF, HALF))
    ph = phase_for(x, 0.0)
    y_path = f"{BOB_AMP}*sin(6.2832*{CYCLES}*t+{ph:.4f})"
    anims.append((cid, y_path))

# --- Volcar entidades -------------------------------------------------------
lines.append("# --- curvas (malla del mar) --------------------------------")
for cid, xe, ye, ze, col, tmin, tmax in entities:
    r, g, b = col
    lines.append(
        f"curve {cid}  {xe}  {ye}  {ze}  "
        f"{r:.3f} {g:.3f} {b:.3f}  {tmin:.4f} {tmax:.4f}  {STROKE}"
    )

lines.append("")
lines.append("# --- bobbing vertical desfasado (animacion path) -----------")
for cid, y_path in anims:
    # path: x_expr y_expr z_expr duration ; offset se suma a la posicion
    lines.append(
        f"animate {cid} position linear path  0  {y_path}  0  {DURATION:.1f}"
    )

lines.append("")

out = "assets/scenes/ocean.txt"
with open(out, "w") as f:
    f.write("\n".join(lines) + "\n")

print(f"Escrito {out}")
print(f"  curvas:      {len(entities)}")
print(f"  animaciones: {len(anims)}")
