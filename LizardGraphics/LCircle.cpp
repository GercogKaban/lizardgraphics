#include "LCircle.h"
#include "LLogger.h"
#include "LApp.h"

LGraphics::LCircle::LCircle(LApp* app)
    :LShape(app,nullptr)
{
    LOG_CALL
        //if (app->info.lighting)
            shader = app->getLightningShader().get();
        //else
            //shader = app->getStandartWorldObjShader();
    projection = app->getProjectionMatrix();
    app->addObjectToCreate(this, L_PRIMITIVE);
}
