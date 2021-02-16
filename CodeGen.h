#include "LizardGraphics/LApp.h"
#include "LizardGraphics/LWRectangle.h"

namespace LGraphics{
void genWidgets(LGraphics::LApp* app)
{
auto rect1 = new LWRectangle(app,"E:/GAMES/lizardgraphics/textures/4k_wood/Wood060_4K_Color.jpg");
rect1->move(0.000000, 0.000000, 0.000000);
rect1->scale(19.285999, 29.266001, 1.000000);
rect1->color(glm::vec3(1.000000, 1.000000, 1.000000));
rect1->setRotate(glm::mat4(-0.005375,0.000000,0.999984,0.000000,0.000000,1.000000,0.000000,0.000000,-0.999984,0.000000,-0.005375,0.000000,0.000000,0.000000,0.000000,1.000000));
rect1->setName("");

}
}
