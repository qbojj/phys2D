#include "GL3_GuiRenderer.h"

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

GL3_GuiRenderer::GL3_GuiRenderer()
{
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init();
}

GL3_GuiRenderer::~GL3_GuiRenderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GL3_GuiRenderer::onDraw( std::vector<PhysicsObject> &objs )
{

}

