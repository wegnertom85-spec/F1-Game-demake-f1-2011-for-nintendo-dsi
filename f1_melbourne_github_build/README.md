# F1 Melbourne DSi - source prototype

This is an original homebrew racing-game prototype scaffold for Nintendo DS/DSi.
It is **not** a modified copy of F1 2011 and does not include Codemasters assets.

## Current prototype scope
- Low-poly 3D closed racing circuit inspired by Albert Park / Melbourne
- TV-style forward onboard camera
- Simple Lotus Renault GP / Mercedes GP team color selection
- Basic acceleration, braking and steering
- No opponent AI, DRS, KERS or menus

## Important limitations
- The track is currently a procedural placeholder loop inspired by Albert Park,
  not an accurate recreation of every real-world corner.
- The two cars are simple colored geometry placeholders, not authentic 2011 car models.
- This is source code, **not a compiled or tested `.nds` ROM**.
- The code is a first prototype and may need small API/build adjustments for the
  exact devkitPro/libnds version and target hardware.

## Build
1. Install devkitPro with the Nintendo DS development packages (`devkitARM`, `libnds`, `nds-dev`).
2. Open a devkitPro MSYS2 terminal.
3. Enter this project directory.
4. Run `make`.
5. If compilation succeeds, the build system produces `f1_melbourne_dsi.nds`.

## Controls
- A: accelerate
- B: brake
- Left / Right: steer
- X: switch Lotus / Mercedes
- START: pause

## Performance notes
The scene uses a small number of low-poly objects and no textures yet. It targets
DS-compatible rendering. The DSi has more RAM than the original DS, but its 3D
hardware is broadly DS-class, so polygon count and overdraw still matter.
