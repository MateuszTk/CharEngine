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
g++ main.cpp `sdl2-config --cflags --libs` -mavx2 -mfma -lpthread -std=c++11 -O2
```

#### Windows:   
Run in ```Developer PowerShell for VS 20xx```
```cmd
cd build
cl /EHsc /MD /arch:AVX2 /O2 ../CharEngine/Examples/main.cpp /I "../external/SDL2-2.30.0\include" /link /LIBPATH:"..\external\SDL2-2.30.0\lib\x86" shell32.lib SDL2.lib SDL2main.lib /SUBSYSTEM:CONSOLE /out:demo.exe
```
