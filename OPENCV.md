To install OpenCV use this script:
```sh
# this can be anywere
mkdir ~/opencv4/
cd ~/opencv4/

# get opencv 4 from it's github repo
git clone https://github.com/opencv/opencv
git -C opencv checkout 4.5.1

# create a 'build' dir in ~/opencv4/
mkdir build
cd ./build/

# compile opencv, this will take a lot of time, get some coffee
make ../opencv/

# install opencv
sudo make install
```
