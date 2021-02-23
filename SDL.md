
To install SDL use this script:  
Adapted from: [here](https://wiki.libsdl.org/Installation)
```sh
# the installation directory is irrelevant
cd ~

# clone SDL into ./sdl directory
hg clone https://hg.libsdl.org/SDL sdl
cd sdl

# create ./build directory
mkdir build
cd build

# build and install SDL
../configure
make
sudo make install
```
