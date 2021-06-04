#include "LizardGraphics/LApp.h"
#include "LizardGraphics/LModel.h"

#include "LizardGraphics/LWRectangle.h"

namespace LGraphics{
void genWidgets(LGraphics::LApp* app)
{
auto model1 = new LModel(app,"E:/GAMES/lizardgraphics/models/dog/13463_Australian_Cattle_Dog_v3.obj","E:/GAMES/lizardgraphics/models/dog/Australian_Cattle_Dog_dif.jpg");
model1->move(0.000000, 0.000000, 0.000000);
model1->scale(0.500000, 0.500000, 1.000000);
model1->color(glm::vec3(0.000000, 0.000000, 0.000000));
model1->setRotate(glm::mat4(1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000));
model1->setName("");
auto rect1 = new LWRectangle(app,"E:/GAMES/lizardgraphics/textures/arbiter.png");
rect1->move(0.000000, 0.000000, 0.000000);
rect1->scale(3.926000, 3.867000, 0.917000);
rect1->color(glm::vec3(1.000000, 1.000000, 1.000000));
rect1->setRotate(glm::mat4(1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000));
rect1->setName("");

}
}
