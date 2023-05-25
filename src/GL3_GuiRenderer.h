#include "interfaces.h"
#include <vector>
#include "PhysicsObject.h"
#include <stdint.h>
#include <glm/glm.hpp>

typedef struct GLFWwindow GLFWwindow;

class GL3_GuiRenderer : public IGuiRenderer {
public:
    GL3_GuiRenderer();
    ~GL3_GuiRenderer();

    bool onDraw( std::vector<PhysicsObject> &, double dt ) override;

private:
    GLFWwindow *window;
    uint32_t buffer;
    uint32_t vao, program;
    uint32_t mViewLocation;

    glm::dvec2 camPos;
    double camZoom;

    bool bTriangles;

    bool object_selected;
    uint32_t object_idx;

    bool pulling_object;
    glm::dvec2 pull_pos;
    double pull_last_angle;

    glm::dvec2 pullArrowDir;
    double pullArrowLen;

    struct VertexInput {
        glm::vec2 vPos;
        glm::u8vec4 vColor;
    };

    void renderScene( std::vector<PhysicsObject> & );

    std::vector<VertexInput> prepareVerts( std::vector<PhysicsObject> & );
    std::vector<uint32_t> renderTriangles( std::vector<PhysicsObject> & );
    std::vector<uint32_t> renderLines( std::vector<PhysicsObject> & );
};