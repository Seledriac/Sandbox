# Sandbox
Sandbox environment with various personal projects and experimentations implemented from scratch (physics simulations, fractals, swarm intelligence, precedural generation, computational geometry, curved space-time rendering, ...). All projects use the same generic UI with interactive plots, automated parameter handling and 3D display scene. The repo is standalone and should not rely on any external library installation.

![Screenshot](Docs/Anim_CFD.gif)
![Screenshot](Docs/Anim_MarkovVoxProcGen.gif)\
![Screenshot](Docs/Anim_FraclElevMapZoom.gif)
![Screenshot](Docs/Anim_FractCurvDev.gif)\
![Screenshot](Docs/Anim_ParticleCollisionConvectionSystem.gif)
![Screenshot](Docs/Anim_ReynoldsBoids.gif)\
![Screenshot](Docs/Anim_AlbertSpaceTimeCurvature.gif)
![Screenshot](Docs/Anim_TerrainErosion.gif)

## Code structure

**main.cpp** contains the main program loop, parameter list display, mouse/keyboard interactions, creation/destruction of the active project

**Data.hpp** contains the parameter list and plot data shared between the UI and each project

**Projects/** contains all projects currently implemented in the sandbox environment. All projects share the same structure with initialization of their parameter list, status flags, refresh/animate/draw methods

**Util/** contains utility functions used by all projects

**Libs/** contains imported libraries

**FileInput/** contains various filtes to be used by the projetcs

**FileOutput/** contains various filtes to created by the projetcs

## Minimal build env on windows
Get gcc compiler and extract at root of C: drive or another appropriate location (e.g. latest Win64 verion at the time of writing : https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-17.0.5-11.0.1-ucrt-r3/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64ucrt-11.0.1-r3.7z)

Doc available at  https://winlibs.com/

Install MinGW at root of C: drive or another appropriate location
https://sourceforge.net/projects/mingw/
Only check "msys-base" package in MinGW Installation Manager to just get bash commands in bin folder

Add to path env var (here shown in the case of C: drive root)
- C:\mingw64\bin
- C:\MinGW\msys\1.0\bin

## Build, run and use
- `make clean` (optional, if rebuild needed)
- `make depend` (optional, if dependencies changed)
- `make` (or `make -j 12` for multithread)
- `./main.exe`
- *use TAB to attach/detach the contextural menou to rightclick*
- *use rightclick to open contextual menu and select project, change options or save configurations*
- *use mouse clics to rotate, pan and zoom with the arcball to move in the 3D scene*
- *use KB arrows, modifier keys, or mouse wheel to highlight and change parameter values in the active project*
- *use > or spacebar to step forward or run animation/simulation if the active project supports it*
- *use number keys to toggle various displays in the active project*
