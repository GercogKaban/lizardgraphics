#include "pch.h"

#include "LScroller.h"
#include "LApp.h"
#include "LText.h"
#include "additional.h"

namespace LGraphics
{
    void LGraphics::LScroller::tick()
    {
        if (app->getActiveWidget() == this)
            moveScrollerToMouse();
        //if (mouseOnIt())
        //    std::cout << "mouse on it\n";
        //else
        //    std::cout << "NO\n";
    }

    LScroller::LScroller(LApp * app, const char * path)
        :LIRectangle(app, path){}

    LGraphics::LVerticalScroller::LVerticalScroller(LApp * app, const char * path)
        : LScroller(app, path){}

    void LVerticalScroller::reloadScroller(size_t scrollPositions)
    {
        move(fvect3(parent->getTopRightCorner().x, parent->move_.y, parent->move_.z));
        positions = scrollPositions;
        scale_.y = parent->getScale().y / float(scrollPositions+1);
        moveScrollerToPos((int)scrollPositions);
    }

    void LVerticalScroller::moveScrollerToMouse()
    {
        if (positions == 0) return;
        float scrollInterval = (parent->getTopRightCorner().y - parent->getBottomRightCorner().y) / ((float)positions+1);

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->getWindowHandler(), &mouse_x, &mouse_y);
        auto mouseYCoord = yOnScreenToGlCoord(fvect2(app->getWindowSize()), (float)mouse_y);
        
        auto begin = parent->getTopRightCorner().y;
        //auto end = parent->getBottomRightCorner().y;

        moveScrollerToPos((int)floor((begin - mouseYCoord) / scrollInterval));
    }

    void LVerticalScroller::moveScrollerToPos(int position)
    {
        if (position < 0 || position > positions)
            return;

        float scrollInterval = (parent->getTopRightCorner().y - parent->getBottomRightCorner().y) / ((float)positions + 1);
        move_.y = parent->getTopRightCorner().y - scrollInterval * (float)position;
        move_.y -= std::abs(getTopRightCorner().y - getBottomRightCorner().y) / 2.0f;
        parent->showFrom(position);
        currentPos = position;
    }

    LGraphics::LHorizontalScroller::LHorizontalScroller(LApp * app, const char * path)
        : LScroller(app, path){}

    void LHorizontalScroller::reloadScroller(size_t scrollPositions)
    {
        //positions = scrollPositions;
        //scale_ = baseScale / scrollPositions;
    }

    void LHorizontalScroller::moveScrollerToMouse()
    {
    }

}