#pragma once

#include "LObject.h"

#include <string>
#include <ft2build.h>
#include "include/GLEW/glew.h"
#include "vectors.h"
//#include "LIRectangle.h"

#include FT_FREETYPE_H 

namespace LShaders
{
    class Shader;
}
namespace LGraphics
{
    class LLine;
    class LText;
    class LApp;
    class LShape;
    class LIRectangle;


    struct Text
    {
        friend LLine;
        friend LShape;
        friend LApp;

        Text(){}
        Text(std::string& text, fvect2 pos, float scale, fvect3 color)
            :text(text), pos(pos), scale(scale), color(color){}

        size_t getTextSize() const { return text.size(); }

        std::string text;
        fvect2 pos = { 0.0f,0.0f };
        float scale = 1.0f;
        float length = 0.0f;
        fvect3 color = { 0.0f,0.0f,0.0f };
        int texture = -1;

    private:
    };

    class LLine : public LObject
    {   
    public:

        const char* getObjectType() override { return "LLine"; }
        friend LText;
        struct Character
        {
            unsigned int textureID;  // ID handle of the glyph texture
            ivect2 size;            // Size of glyph
            ivect2 bearing;         // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };

    protected:

        static LIRectangle* textRect;

        static glm::mat4 projection;
        static LShaders::Shader* symbShader;
        static LShaders::Shader* textRectShader;
        static Character characters[CHAR_MAX + 1];
        static GLuint VAO, VBO;
        static LGraphics::LApp* app;

        static unsigned int stringFBO, stringTexture;

    public:

        LLine(LApp* app);
        ~LLine();

        static LShaders::Shader* getShader() { return symbShader; }
        static void display(const std::string text, float x, float y, const float scale, const fvect3 color);
        static void display(Text& text);

        static void displayStaticText(std::vector<Text> textObjects, bool& refresh);

        static float getTextLength(const Text& text);
    };
}
