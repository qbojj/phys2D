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

## Materiały

Wykorzystane zostały materiały:
- obliczanie otoczki wypukłej: https://en.wikipedia.org/wiki/Graham_scan
- obliczanie środka obiektu: https://en.wikipedia.org/wiki/Centroid
- obliczanie momentu bezwładności obitektu: https://physics.stackexchange.com/questions/708936/how-to-calculate-the-moment-of-inertia-of-convex-polygon-two-dimensions
- obliczanie reakcji na kolizję: https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdfS

## Zewnętrzne biblioteki

- Dynamiczne ładowanie biblioteki OpenGL: [glad](https://github.com/Dav1dde/glad)
- Akcelaracja sprzętowa grafiki: [OpenGL](https://www.opengl.org/)
- Algebra liniowa: [glm](https://github.com/g-truc/glm)
- gui: 
  - [glfw](https://github.com/glfw/glfw)
  - [imgui](https://github.com/ocornut/imgui)
