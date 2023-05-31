/*
Jakub Janeczko
main...
31.05.2023
*/

#include "app.h"

#include "GL3_Renderer.h"
#include "Simple_Gui.h"
#include "Simple_PhysicsEngine.h"

#include <imgui.h>
#include <memory>
#include <iostream>

int main()
{
    if( !ImGui::CreateContext() )
    {
        std::cerr << "imgui context cound not be created\n";
        return -1;
    }

    {
        auto engine = std::make_unique<Simple_PhysicsEngine>();
        auto renderer = std::make_unique<GL3_Renderer>();
        
        auto gui = std::make_unique<Simple_Gui>( engine.get() );

        App app(
            GuiRenderer{ gui.get(), renderer.get() },
            engine.get() );
        
        app.run();
    }
    
    ImGui::DestroyContext();
}
