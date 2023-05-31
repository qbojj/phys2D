/*
Jakub Janeczko
header klasy GUI
31.05.2023
*/

#pragma once

#include "interfaces.h"
#include "Simple_PhysicsEngine.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

/**
 * @brief GUI z możliwością manipulacji Simple_PhysicsEngine
 * 
 * posiada możliwość:
 * - dodawania i usuwania obiektów
 * - zmiany parametrów Simple_PhysicsEngine
 * - ciągnięcia obiektów
 * - zobaczenia wewnętrznych parametrów obiektów
 * - wypisuje jak długo zajmuje przejście do kolejnego stanu
 */
class Simple_Gui : public IGui {
public:
    /**
     * @brief tworzy GUI operujące na podanym silniku
     * 
     * @param engine silnik fizyki do modyfikacji parametrów
     */
    Simple_Gui( Simple_PhysicsEngine *engine );

    virtual renderer_info handle_gui( std::vector<PhysicsObject> &objs, double dt );

private:
    glm::dvec2 camPos;
    double camZoom;

    bool bTriangles;

    bool object_selected;
    uint32_t object_idx;

    bool pulling_object;
    glm::dvec2 pull_pos;
    double pull_last_angle;

    bool creation_mode;
    double density;
    std::vector<glm::dvec2> point_cloud;
    int flags_created_item;

    bool error_popup;
    std::string error_message;

    bool pulling_scene;

    Simple_PhysicsEngine *engine;
};