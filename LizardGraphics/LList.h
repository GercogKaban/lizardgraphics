#pragma once

#include "LIButton.h"

namespace LGraphics
{
    class LList : public LIButton
    {
    public:
        friend LApp;

        enum ListOrientations {
            bottom = 0,
            top = 1,
            left = 2,
            right = 3
        };

        const char* getObjectType() override { return "LList"; }
        LList(LApp* app, const char* path = nullptr);

        void addInnerWidget(LWidget* w) override;
        void setHorizOrientation(int orientation);
        void setVertOrientation(int orientation);
        ivect2 getListOrientations() const { return { horizListOrientation,vertListOrientation }; }


    protected:

        fvect2 getLastWidgetMove() const;
        void calculateNextIntend();

        int horizListOrientation = left;
        int vertListOrientation = bottom;

        void refresh();
        float calculateXIntend(LRectangleShape* w) const;
        float calculateYIntend(LRectangleShape* w) const;

        float intend = 0.05f;
        fvect2 nextMove = fvect2(0.0f);
    };
}
