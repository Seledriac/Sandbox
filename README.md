# Sandbox
Sandbox project for all sort of stuff implemented from scratch

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
