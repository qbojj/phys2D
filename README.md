# phys2D
czyli prosty silnik fizyki 2D z wizualizacją

## kompilacja

### Ubuntu
```sh
sudo apt-get install -y cmake xorg-dev
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Windows
potrzeba zainstalować CMake oraz Visual Studio lub MinGW
```bat
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
