#include "pch.h"

namespace LGraphics
{
    bool isPointInPolygon(int npol, float* xp, float* yp, fvect2 p)
    {
        bool c = false;
        for (int i = 0, j = npol - 1; i < npol; j = i++)
        {
            if ((
                (yp[i] < yp[j]) && (yp[i] <= p.y) && (p.y <= yp[j]) &&
                ((yp[j] - yp[i]) * (p.x - xp[i]) > (xp[j] - xp[i]) * (p.y - yp[i]))
                ) || (
                (yp[i] > yp[j]) && (yp[j] <= p.y) && (p.y <= yp[i]) &&
                    ((yp[j] - yp[i]) * (p.x - xp[i]) < (xp[j] - xp[i]) * (p.y - yp[i]))
                    ))
                c = !c;
        }
        return c;
    }

    fvect2 pointOnScreenToGlCoords(fvect2 screenSize, fvect2 point)
    {
        point.y = screenSize.y - point.y;
        return point / screenSize - (fvect2(1.0f, 1.0f) - point / screenSize);
    }

    fvect2 glCoordsToScreenCoords(fvect2 screenSize, fvect2 val)
    {
        return ((val + fvect2(1.0f, 1.0f))*screenSize) / 2.0f;
    }

    float xOnScreenToGlCoord(fvect2 screenSize, float x)
    {
        return x / screenSize.x - (1.0f - x / screenSize.x);
    }

    float yOnScreenToGlCoord(fvect2 screenSize, float y)
    {
        y = screenSize.y - y;
        return y / screenSize.y - (1.0f - y / screenSize.y);
    }

    float xGlCoordToScreenCoord(fvect2 screenSize, float x)
    {
        return ((x + 1.0f)*screenSize.x) / 2.0f;
    }

    float yGlCoordToScreenCoord(fvect2 screenSize, float y)
    {
        return ((y + 1.0f)*screenSize.y) / 2.0f;
    }

    fvect3 rgbToFloat(unsigned char r, unsigned char g, unsigned char b)
    {
        return{ (float)r / (float)sizeof(unsigned char), (float)g / (float)sizeof(unsigned char), (float)b / (float)sizeof(unsigned char) };
    }
}

//#ifdef __linux__
//
//    std::string getClipboard()
//    {
//        return std::string();
//    }
//
//#elif _WIN32
//
//    std::string getClipboard()
//    {
//        // Try opening the clipboard
//        if (!OpenClipboard(nullptr))
//            throw std::exception("can't open clipboard\n");
//
//          // Get handle of clipboard object for ANSI text
//        HANDLE hData = GetClipboardData(CF_TEXT);
//        if (hData == nullptr)
//            throw std::exception("can't get data handler\n");
//
//          // Lock the handle to get the actual text pointer
//        char * pszText = static_cast<char*>(GlobalLock(hData));
//        if (pszText == nullptr)
//            throw std::exception("can't get text by handler\n");
//
//        std::string text(pszText);
//        GlobalUnlock(hData);
//        CloseClipboard();
//
//        return text;
//    }
//
//#endif