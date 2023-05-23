#include "interfaces.h"
#include <vector>
#include "PhysicsObject.h"

typedef struct GLFWwindow GLFWwindow;

class GL3_GuiRenderer : public IGuiRenderer {
public:
    GL3_GuiRenderer();
    ~GL3_GuiRenderer();

    void onDraw( std::vector<PhysicsObject> & ) override;

private:
    GLFWwindow *window;
};