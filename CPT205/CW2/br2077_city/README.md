# Blade Runner 2077 Style City (OpenGL / FreeGLUT)

This is a teaching/demo project for an assignment. It builds a small night-time cyberpunk city
inspired by *Blade Runner 2049* style locations:

- LAPD brutalist tower in the city center
- Chaotic downtown with high-rises, roads, bridges, neon billboards
- New Los Angeles junkyard (Trash Mesa) with hovering garbage ships
- Orphanage + factory with giant exhaust fans
- Radioactive casino ruins in the west
- Sea wall and ocean with red aircraft warning beacons
- Memory laboratory with snowy staircase scene in the north
- Floating police spinners following simple paths above the city

The implementation follows a fixed-function OpenGL pipeline using FreeGLUT.

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
./br2077_city   # or br2077_city.exe on Windows
```

You must have OpenGL and FreeGLUT (or GLUT compatible) libraries available on your system.

## Controls

- `W/S` : move forward / backward
- `A/D` : strafe left / right
- `Q/E` : move down / up (limited range)
- Arrow keys : rotate view (left/right yaw, up/down pitch)
- `R` : reset camera to the starting position
- `ESC` : exit

Camera movement includes simple collision against main buildings and map bounds.

## Textures

The code expects a few BMP textures in the `textures/` folder:

- `concrete.bmp`
- `concrete_dark.bmp`
- `windows_blue.bmp`
- `windows_warm.bmp`
- `road.bmp`
- `metal_junk.bmp`
- `neon1.bmp`
- `neon2.bmp`
- `ocean.bmp`
- `sky_night.bmp`
- `rust.bmp`

They are **not** included here. You should create them yourself using any painting program
(simple color gradients and noise patterns are enough). The loader only supports 24‑bit uncompressed
BMP files. If loading fails, a fallback 1x1 white texture is created automatically.

## Notes

- The city map is a square (roughly 1000x1000 units) divided into logical zones.
- Fog and lighting are used to limit visible distance and separate different regions
  (downtown rain / junkyard haze / orange radiation zone / snowy memory lab).
- The geometry is intentionally kept simple (boxes, cylinders, ramps) to match typical
  introductory computer graphics assignments.
