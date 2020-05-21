#pragma once
//#include "LRectangleShape.h"
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H 

namespace LGraphics
{
    class LSymbol
    {
    public:
        LSymbol()
        {
            FT_Library ft;
            if (FT_Init_FreeType(&ft))
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

            FT_Face face;
            if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }
    };
    //{
    //    friend LRectangleShape;
    //public:
    //    LSymbol(LApp* app);
    //protected:
    //    // эта переменная не работает, передается мусор :(
    //    //const char fontPath[37] = "../lizardgraphics/res/font_white.png";
    //    void setTextureBuffer();
    //};
}
