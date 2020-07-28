#pragma once

#include "LIRectangle.h"

namespace LGraphics
{
    class LScroller : public LIRectangle
    {
    public:

        friend LText;
        friend LApp;

        void tick() override;

    protected:

        LScroller(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);

        void setParent(LText* parent_) { this->parent = parent_; }

        virtual void moveScrollerToMouse() {}
        virtual void moveScrollerToPos(int position) = 0;

        virtual void reloadScroller(size_t scrollPositions) = 0;

        size_t currentPos = 0;
        size_t positions = 0;
        LText* parent;
    };

    class LVerticalScroller : public LScroller
    {
    public:

        friend LText;

        LVerticalScroller(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);

    protected:

        void reloadScroller(size_t scrollPositions) override;
        void moveScrollerToMouse() override;
        void moveScrollerToPos(int position) override;
    };

    class LHorizontalScroller : public LScroller
    {
    public:
        friend LText;

        LHorizontalScroller(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBaseComponent* component = nullptr);
        void reloadScroller(size_t scrollPositions) override;

    protected:

        void moveScrollerToMouse() override;
    };

}

