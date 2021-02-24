#include "LizardGraphics/LApp.h"
#include "LizardGraphics/LModel.h"

namespace LGraphics{
void genWidgets(LGraphics::LApp* app)
{
auto model1 = new LModel(app,"models/dog/Wolf_obj.obj","models/dog/Australian_Cattle_Dog_dif.jpg");
model1->move(0.000000, 10.000000, 0.000000);
model1->scale(0.500000, 0.500000, 1.000000);
model1->color(glm::vec3(0.000000, 0.000000, 0.000000));
model1->setRotate(glm::mat4(-1.000000,0.000000,-0.000000,0.000000,0.000000,0.000000,-1.000000,0.000000,0.000000,-1.000000,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000));
model1->setName("");

}
}
