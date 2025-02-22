# Char Engine
Char Engine is an implementation of a *simple* software renderer.   
Features:
- Textures
- Transparency
- Object loading
- AVX acceleration  
- Multithreaded rendering   


## Building 
Use the following commands in the ./CharEngine/Examples directory to compile the Char Engine sample project   
#### Linux:   
If you don't have SDL install it first:  
```sh
sudo apt-get install libsdl2-ttf-dev
```
Then build with:
```sh
g++ main.cpp `sdl2-config --cflags --libs` -mavx2 -mfma -lpthread -std=c++11
```

#### Windows:   
If you don't have SDL [install](https://github.com/libsdl-org/SDL/releases) it first.    
Then build with:
```cmd
cl /EHsc /MD /arch:AVX2 /O2 main.cpp /I "SDL2-2.0.14\include" /link /LIBPATH:"SDL2-2.0.14\lib\x86" shell32.lib SDL2.lib SDL2main.lib /SUBSYSTEM:CONSOLE /out:main.exe
```

![CharEngine_Car](https://github.com/MateuszTk/CharEngine/blob/master/screenshots/car.jpg?raw=true)   
![CharEngine_Ship](https://github.com/MateuszTk/CharEngine/blob/master/screenshots/ship.jpg?raw=true)
