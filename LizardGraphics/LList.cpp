#include "LList.h"

LGraphics::LList::LList(LApp * app, const char * path)
    :LIButton(app,path)
{
    setClickEventFunction([&]()
    {
        if (getInnerWidgetsPtr() && getInnerWidgets().size())
            for (auto& i : getInnerWidgets())
                i->setHidden(!i->isHidden());
    });
}

void LGraphics::LList::addInnerWidget(LWidget* w)
{
    calculateNextIntend();
    w->move(fvect3{nextMove.x, nextMove.y, 0.0f });
    w->scale(getScale());
    LWidget::addInnerWidget(w);
}

void LGraphics::LList::setHorizOrientation(int orientation)
{
    horizListOrientation = orientation;
    refresh();
}

void LGraphics::LList::setVertOrientation(int orientation)
{
    vertListOrientation = orientation;
    refresh();
}

fvect2 LGraphics::LList::getLastWidgetMove() const
{
    return !innerWidgets? getMove().xy: innerWidgets->back()->getMove().xy;
}

void LGraphics::LList::calculateNextIntend()
{
    auto innerW = innerWidgets? (LRectangleShape*)innerWidgets->back() : nullptr;
    nextMove = { calculateXIntend(innerW),calculateYIntend(innerW) };
}

void LGraphics::LList::refresh()
{
    if (innerWidgets)
    {
        auto firstW = (LRectangleShape*)innerWidgets->operator[](0);
        firstW->move(fvect3{ calculateXIntend(nullptr), getMove().y,0.0f });
        for (size_t i = 1; i < innerWidgets->size(); ++i)
        {
            auto prev = (LRectangleShape*)innerWidgets->operator[](i - 1);
            innerWidgets->operator[](i)->move(
                fvect3{ prev->getMove().x, calculateYIntend(prev), 0.0f });
        }
    }
}

float LGraphics::LList::calculateXIntend(LRectangleShape* w) const
{
    return !w ? horizListOrientation == right ? getMove().x + (getTopRightCorner().x - getTopLeftCorner().x) : 
        getMove().x - (getTopRightCorner().x - getTopLeftCorner().x)
        :w->getMove().x;


        //    getMove().x - (getTopRightCorner().x - getTopLeftCorner().x)
    //if (!w)
    //    return horizListOrientation == right ? getMove().x + (getTopRightCorner().x - getTopLeftCorner().x) :
    //    getMove().x - (getTopRightCorner().x - getTopLeftCorner().x);
    //else
    //    return w->getMove().x;
}
float LGraphics::LList::calculateYIntend(LRectangleShape* w) const
{
    return !w? 0.0f: vertListOrientation == top ? w->getMove().y + (w->getTopLeftCorner().y - w->getBottomLeftCorner().y) :
            w->getMove().y - (w->getTopLeftCorner().y - w->getBottomLeftCorner().y);
    //if (!w)
    //    return 0.0f;
    //else
    //    //auto w = (LRectangleShape*)innerWidgets->back();
    //    return vertListOrientation == top ? w->getMove().y + (w->getTopLeftCorner().y - w->getBottomLeftCorner().y) :
    //        w->getMove().y - (w->getTopLeftCorner().y - w->getBottomLeftCorner().y);
}
