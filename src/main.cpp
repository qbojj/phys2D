/*
Jakub Janeczko
main...
28.05.2023
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
        App app(
            GuiRenderer{
                std::make_unique<Simple_Gui>(), 
                std::make_unique<GL3_Renderer>() },
            std::make_unique<Simple_PhysicsEngine>( 9.81 ) );
        
        app.run();
    }
    
    ImGui::DestroyContext();
}
