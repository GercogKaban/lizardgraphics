#include "LFlag.h"
#include "LApp.h"
#include "textures.h"

LGraphics::LFlag::LFlag(LApp * app, const char * path)
    :LIButton(app,path)
{
    scale({ 0.025f,0.03f,1.0f });
    setClickEventFunction([&]() {switchState();});
    setShader(app->getStandartCheckMarkShader());
    bindTexture(checkMark, checkMarkSize);
    isInited_ = true;
    turnOffTexture();
}

void LGraphics::LFlag::switchState()
{
    setState(!state);
}

void LGraphics::LFlag::setState(bool state)
{
    this->state = state;
    if (state)
    {
        turnOnTexture();
        activeAction();
    }
    else
    {
        turnOffTexture();
        unactiveAction();
    }
}

void LGraphics::LFlag::setActiveAction(std::function<void()> action)
{
    activeAction = action;
}

void LGraphics::LFlag::setUnactiveAction(std::function<void()> action)
{
    unactiveAction = action;
}
