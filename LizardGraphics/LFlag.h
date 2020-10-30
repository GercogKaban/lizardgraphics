#pragma once
#include "LIButton.h"

namespace LGraphics
{
    class LFlag : public LIButton
    {
    public:

        const char* getObjectType() override { return "LFlag"; }

        LFlag(LApp* app, const char* path = nullptr);

        void switchState();
        void setActiveAction(std::function<void()> action);
        void setUnactiveAction(std::function<void()> action);
        virtual void setState(bool state);

        bool isActive() const { return state; }

    private:

        bool state = false;
        std::function<void()> activeAction = [](){};
        std::function<void()> unactiveAction = []() {};
    };
}