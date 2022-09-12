# Char Engine
Char Engine is an implementation of a *simple* software renderer.   
Features:
- Textures
- Transparency
- Object loading
- AVX acceleration (Wow!)  
- Multithreaded rendering

See some epic examples in `./screenshots/`

## Building 
Use this command in the ./CharEngine/Examples directory to compile Char Engine:   
#### Linux: (TODO)
If you don't have SDL [install](./SDL.md) it first.  
```sh
g++ main.cpp `sdl2-config --cflags --libs` -mavx -lpthread -std=c++11
```

#### Windows:
If you don't have SDL [install](https://github.com/libsdl-org/SDL/releases) it first. 
```cmd
cl /EHsc /MD /arch:AVX2 /O2 main.cpp /I "SDL2-2.0.14\include" /link /LIBPATH:"SDL2-2.0.14\lib\x86" shell32.lib SDL2.lib SDL2main.lib /SUBSYSTEM:CONSOLE /out:main.exe
```
