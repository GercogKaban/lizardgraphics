#include "LizardGraphics/LApp.h"
#include "LizardGraphics/LWRectangle.h"

namespace LGraphics{
void genWidgets(LGraphics::LApp* app)
{
auto rect1 = new LWRectangle(app,"E:/GAMES/lizardgraphics/textures/arbiter.png");
rect1->move(0.000000, 0.000000, 0.000000);
rect1->scale(0.500000, 0.500000, 1.000000);
rect1->color(glm::vec3(1.000000, 1.000000, 1.000000));
rect1->setRotate(glm::mat4(1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000));
rect1->setName("");

}
}
