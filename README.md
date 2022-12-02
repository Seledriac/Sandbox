# Sandbox
Sandbox project for all sort of stuff implemented from scratch

## Minimal build env on windows
Install MinGW, e.g. in "C:\MinGW"

Check packages in MinGW Installation Manager
- mingw-developer-toolkit
- mingw32-base
- mingw32-gcc-g++
- msys-base            (bash commands in "C:\MinGW\msys\1.0\bin")
- mingw32-pthreads-w32 (multithreading with OpenMP)

Add to path
- C:\MinGW\msys\1.0\bin
- C:\MinGW\bin

## Build and run sandbox
- make clean
- make depend
- make
- ./main
