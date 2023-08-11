# Sandbox
Sandbox environment with various projects and experimentations (fractals, physics simulations, computational geometry, structural optimization...) implemented from scratch. All projects use a generic UI with interactive plots, automated parameter handling and 3D display scene. The repo is standalone and does not rely on any external library.

## Code structure

**main.cpp** contains the main program loop, parameter list display, mouse/keyboard interations, creation/destruction of the active project.

**Data.hpp** contains the parameter list and plot data shared between the UI and each project

**util/** contains header-only utility namespaces for colormaps, random numbers, vectors, fields, ...

**Projects/** contains all projects currently implemented in the sandbox environment. All projects share the same structure with initialization of they parameter lists, status flags, refresh/animate/draw methods

## Minimal build env on windows
Get Win64 gcc compiler and extract at root of C: drive
https://github.com/brechtsanders/winlibs_mingw/releases/download/12.2.0-14.0.6-10.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-12.2.0-mingw-w64ucrt-10.0.0-r2.7z
Doc available at  https://winlibs.com/

Install MinGW at root of C: drive
https://sourceforge.net/projects/mingw/
Only check "msys-base" package in MinGW Installation Manager to get bash commands in bin folder

Add to path env var
- C:\mingw64\bin
- C:\MinGW\msys\1.0\bin

## Build and run sandbox
- make clean
- make depend
- make
- ./main
- *press any of the alphabet key to make the assotiated project active*
