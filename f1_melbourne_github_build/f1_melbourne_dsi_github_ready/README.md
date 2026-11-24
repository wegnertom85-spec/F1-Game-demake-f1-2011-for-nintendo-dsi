# F1 Melbourne DSi – homebrew prototype

An original Nintendo DS/DSi homebrew prototype. It does not contain extracted,
modified, or redistributed commercial F1 2011 game data.

## Current build target
- DS-compatible `.nds` output (also intended to run on DSi)
- Hardware 3D scene
- Onboard-style forward camera with a simple camera-space car nose
- Basic acceleration, braking, steering
- X toggles Lotus-inspired gold/black and Mercedes-inspired silver/turquoise colors
- Procedural low-poly loop and lightweight scenery
- No opponent AI, DRS, KERS, or menus

## Important status
This is an early technical prototype, not a finished F1 game. The track is a
procedural placeholder inspired by Albert Park, not an accurate recreation of
the complete real circuit. Car bodies are simple original placeholder geometry,
not authentic team models. Performance and boot behavior still need testing on
a real DSi or emulator.

## Build locally
Use devkitPro with devkitARM and libnds installed, then run `make` in this folder.

## Build on GitHub Actions
The workflow at `.github/workflows/build-nds.yml` compiles on pushes to `main`
or `master`, and can also be started manually from the Actions tab. On a
successful run, download the `f1-melbourne-dsi` artifact.

If the workflow does not appear, make sure `.github/workflows/build-nds.yml`
was uploaded with the leading-dot `.github` directory intact.
