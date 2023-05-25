#include "app.h"
#include "GL3_GuiRenderer.h"
#include "Simple_PhysicsEngine.h"
#include <memory>

int main()
{
    App app(
        std::make_unique<GL3_GuiRenderer>(),
        std::make_unique<Simple_PhysicsEngine>() );
    
    app.run();
}
