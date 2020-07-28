#pragma once
//#include "LRectangleShape.h"
#include <map>
#include <iostream>
#include <array>

#include <ft2build.h>
#include "include/GLEW/glew.h"

#include "LTimer.h"
#include "vectors.h"
#include FT_FREETYPE_H 

namespace LShaders
{
    class Shader;
}
namespace LGraphics
{
    class LText;
    class LApp;

    struct Text
    {
        std::string text; 
        fvect2 pos = { 0.0f,0.0f };
        float scale = 1.0f;
        float length = 0.0f;
        fvect3 color = {0.0f,0.0f,0.0f};
    };

    class LLine
    {   
    public:
        friend LText;
        struct Character
        {
            unsigned int textureID;  // ID handle of the glyph texture
            ivect2 size;            // Size of glyph
            ivect2 bearing;         // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };

    protected:

        static LShaders::Shader* shader;
        static Character characters[CHAR_MAX + 1];
        static unsigned int VAO, VBO;
        static LGraphics::LApp* app;

    public:

        LLine(LApp* app);

        static LShaders::Shader* getShader() { return shader; }
        static void display(const std::string text, float x, float y, const float scale, const fvect3 color);
        static void display(Text text);
        static float getTextLength(Text text);
    };
}
