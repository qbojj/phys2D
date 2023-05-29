/*
Jakub Janeczko
definicje interfejsów
28.05.2023
*/

#pragma once

#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>

struct renderer_info {
    bool fill_objects;
    std::vector<glm::dvec2> additional_points;
    std::vector<glm::dvec2> additional_lines;
    std::vector<glm::dvec2> additional_triangles;
    glm::dvec2 camPos;
    double camZoom;

    std::vector<glm::u8vec4> object_colors;
};

class IGui {
public:
    virtual ~IGui() = default;
    virtual renderer_info handle_gui( std::vector<PhysicsObject> &objs, double dt ) = 0;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    // return false if should close
    virtual bool draw( const renderer_info &ri,
        const std::vector<PhysicsObject> &objs, double dt ) = 0;
};

class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;
    virtual void onTick( std::vector<PhysicsObject> &objs, double dt ) = 0;
};