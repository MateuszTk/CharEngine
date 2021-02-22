# Chad Engine

## Building 
Use this command in the ./ChadEngine directory to compile ChadEngine:  
If you don't have OpenCV [install](./OPENCV.md) it first.
```sh
g++ -I/usr/local/include/ -I/usr/local/include/opencv4 -L/usr/local/lib/ -L/usr/local/bin/ CharEngine.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -mavx -lpthread -std=c++11
```
