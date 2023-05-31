/*
Jakub Janeczko
nagłówek Renderera
31.05.2023
*/

#pragma once

#include "interfaces.h"

typedef struct GLFWwindow GLFWwindow;

/**
 * @brief renderer korzystający z openGl 3 oraz GLFW
 * 
 * posiada możliwość wyświetlania obwodów lub wypełnionych obiektów
 */
class GL3_Renderer : public IRenderer {
public:
    GL3_Renderer();
    ~GL3_Renderer();

    virtual bool draw( const renderer_info &ri,
        const std::vector<PhysicsObject> &objs, double dt );

private:
    GLFWwindow *window;
    uint32_t buffer;
    uint32_t vao, program;
    uint32_t mViewLocation;

    struct VertexInput {
        glm::vec2 vPos;
        glm::u8vec4 vColor;
    };

    std::vector<VertexInput> prepareVerts( const renderer_info &,
        const std::vector<PhysicsObject> & );
        
    std::vector<uint32_t> renderTriangles( const std::vector<PhysicsObject> & );
    std::vector<uint32_t> renderLines( const std::vector<PhysicsObject> & );
};