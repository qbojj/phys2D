# phys2D

czyli prosty silnik fizyki 2D z wizualizacją

## kompilacja

by otrzymać źródła:

```sh
git clone --recurse-submodules --shallow-submodules https://github.com/qbojj/phys2D.git
cd phys2D
```

### Ubuntu

```sh
sudo apt-get install -y cmake xorg-dev
mkdir build
cd build
cmake ..
cmake --build . --config=Release
```

### Windows

potrzeba zainstalować CMake oraz Visual Studio lub MinGW

```bat
mkdir build
cd build
cmake ..
cmake --build . --config=Release
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
  - Otwieranie okna i kontekstu OpenGL: [glfw](https://github.com/glfw/glfw)
  - Gui aplikacji: [imgui](https://github.com/ocornut/imgui)
