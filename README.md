# Chad Engine
Chad Engine is an implementation of a *simple* software renderer. It's comparable to TGL.   
Features:
- Textures
- Transparency
- Object loading
- AVX acceleration (Wow!)  

See some epic examples in `./screenshots/`

## Building 
Use this command in the ./CharEngine directory to compile Chad Engine:  
If you don't have OpenCV [install](./OPENCV.md) it first.
```sh
g++ -I/usr/local/include/ -I/usr/local/include/opencv4 -L/usr/local/lib/ -L/usr/local/bin/ CharEngine.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -mavx -lpthread -std=c++11
```
