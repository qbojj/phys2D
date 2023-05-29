#pragma once

#include "interfaces.h"

typedef struct GLFWwindow GLFWwindow;

class GL3_Renderer : public IRenderer {
public:
    GL3_Renderer();
    ~GL3_Renderer();

    bool draw( const renderer_info &ri,
        const std::vector<PhysicsObject> &objs, double dt ) override;

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