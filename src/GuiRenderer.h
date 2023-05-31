/*
Jakub Janeczko
nagłówek klasy łączącej GUI i Renderer
31.05.2023
*/

#pragma once

#include "interfaces.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>
#include <stdint.h>
#include <memory>

/**
 * @brief klasa łącząca renderer i GUI
 */
class GuiRenderer {
public:
    GuiRenderer( IGui *gui, IRenderer *renderer )
        : gui(gui), renderer(renderer) {}
    
    /**
     * @brief obsługuje interakcje z użytkownikiem i rysuje GUI wraz z obiektami
     * 
     * @param objs obiekty w symulacji
     * @param dt różnica czasu od poprzedniego kroku symulacji
     * @return true program powinien kontynuować działanie
     * @return false program powinien się zakończyć
     */
    bool onDraw( std::vector<PhysicsObject> &objs, double dt );

private:
    IGui *gui;
    IRenderer *renderer;
};