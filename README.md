# Chad Engine
Chad Engine is an implementation of a *simple* software renderer. It's comparable to TGL.   
Features:
- Textures
- Transparency
- Object loading
- AVX acceleration (Wow!)  
- Multithreaded rendering

See some epic examples in `./screenshots/`

## Building 
Use this command in the ./CharEngine directory to compile Chad Engine:  
If you don't have SDL [install](./SDL.md) it first.
```sh
g++ CharEngine.cpp `sdl2-config --cflags --libs` -mavx -lpthread -std=c++11
```
