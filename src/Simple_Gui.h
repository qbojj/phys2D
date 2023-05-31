/*
Jakub Janeczko
header klasy GUI
30.05.2023
*/

#pragma once

#include "interfaces.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

class Simple_Gui : public IGui {
public:
    Simple_Gui();

    renderer_info handle_gui( std::vector<PhysicsObject> &objs, double dt ) override;

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
};