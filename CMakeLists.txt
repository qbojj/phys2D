# Jakub Janeczko
# konfiguracja kompilacji projektu
# 03.06.2023

# wymagany jest kompilator C++ (MSVC / GCC / CLANG) oraz cmake >= 3.20
# by zkompilować projekt:
#
# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Release ..
# cmake --build .

cmake_minimum_required(VERSION 3.20)
project( phys2D C CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set( GLFW_BUILD_EXAMPLES OFF )
set( GLFW_BUILD_TESTS OFF )
set( GLFW_BUILD_DOCS OFF )
set( GLFW_INSTALL OFF )
add_subdirectory( "deps/glfw" )
add_subdirectory( "deps/glm" )

file(GLOB imgui_SRC CONFIGURE_DEPENDS
  "deps/imgui/*.cpp"
  "deps/imgui/backends/imgui_impl_opengl3.cpp"
  "deps/imgui/backends/imgui_impl_glfw.cpp"
)

add_library( imgui ${imgui_SRC} )
target_include_directories( imgui
  PUBLIC "deps/imgui" "deps/imgui/backends"
)
target_link_libraries( imgui PRIVATE glfw )

add_library( glad "deps/glad/src/gl.c" )
target_include_directories( glad
  PUBLIC "deps/glad/include"
)

file(GLOB phys2D_SRC CONFIGURE_DEPENDS
  "src/*.h"
  "src/*.cpp"
)

add_executable( phys2D ${phys2D_SRC} )
target_compile_options( phys2D PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_include_directories( phys2D PRIVATE
  "src/"
)
target_link_libraries( phys2D PRIVATE
  imgui glm::glm glfw glad
)

configure_file(
  ${CMAKE_SOURCE_DIR}/times.ttf
  ${CMAKE_CURRENT_BINARY_DIR}/times.ttf
  COPYONLY )