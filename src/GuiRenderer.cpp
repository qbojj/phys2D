/*
Jakub Janeczko
klasa łącząca GUI i Renderer
30.05.2023
*/

#include "GuiRenderer.h"

bool GuiRenderer::onDraw( std::vector<PhysicsObject> &objs, double dt )
{
    const renderer_info ri = gui->handle_gui( objs, dt );
    return renderer->draw( ri, objs, dt );
}