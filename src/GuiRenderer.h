/*
Jakub Janeczko
nagłówek klasy implementującej GUI
28.05.2023
*/

#pragma once

#include "interfaces.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>
#include <stdint.h>
#include <memory>

class GuiRenderer {
public:
    GuiRenderer( std::unique_ptr<IGui> gui, std::unique_ptr<IRenderer> renderer )
        : gui(std::move(gui)), renderer(std::move(renderer)) {}
    
    bool onDraw( std::vector<PhysicsObject> &objs, double dt );

private:
    std::unique_ptr<IGui> gui;
    std::unique_ptr<IRenderer> renderer;
};