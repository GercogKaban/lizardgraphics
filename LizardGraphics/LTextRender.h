#pragma once

#include "LObject.h"

#include <string>
#include <ft2build.h>
#include "include/GLEW/glew.h"
#include "vectors.h"

//#include "LApp.h"
//#include "LIRectangle.h"

#include FT_FREETYPE_H 

namespace LShaders
{
    class Shader;
}
namespace LGraphics
{
    class LApp;
    class LTextRender;
    class LText;
    class LShape;
    class LIRectangle;

    enum RefreshModes
    {
        Static,
        Rare,
        Dynamic,
    };


    struct Text
    {
        friend LShape;
        friend LApp;
        friend LTextRender;

        Text(){}
        Text(std::string text, fvect2 pos, float scale, fvect3 color)
            :text(text), pos(pos), scale(scale), color(color)
        {
            //app->addText(this);
        }

        void setColor(fvect3 color) { this->color = color; refreshed = true; }
        void setPos(fvect2 pos) { this->pos = pos; refreshed = true;}
        void setScale(float scale) { this->scale = scale; refreshed = true; }
        void setText(std::string text) { this->text = text; refreshed = true; }
        void setLength(float length) { this->length = length; refreshed = true; }

        bool haveRefreshed() const { return refreshed; }
        fvect3 getColor() const { return color; }
        fvect2 getPos() const { return pos; }
        float getScale() const { return scale; }
        const std::string& getText() const { return text; }
        std::string getTextCopy() { return text; }
        //size_t getTextSize() const { return text.size(); }
        float getLength() const { return length; }

    private:

        std::string text;
        fvect2 pos = { 0.0f,0.0f };
        float scale = 1.0f;
        float length = 0.0f;
        fvect3 color = { 0.0f,0.0f,0.0f };
        bool refreshed = false;
    };

    class LTextRender : public LObject
    {   
    public:

        const char* getObjectType() override { return "LLine"; }
        friend LText;
        friend LApp;

        struct Character
        {
            unsigned int textureID;  // ID handle of the glyph texture
            ivect2 size;            // Size of glyph
            ivect2 bearing;         // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };
        

        LTextRender(LApp* app);
        ~LTextRender();

        static LShaders::Shader* getShader() { return symbShader; }
        static void display(const std::string& text, fvect2 pos, float scale, const fvect3 color, const szvect2 screenOrTextureSize);
        //static void display(Text& text);
        //static void displayText(const std::vector<Text>& textObjects, int refreshMode);

        static void displayText();
        static void addText(Text* text);

        static float getTextLength(const Text& text);

    protected:

        static void genTexture(int num, fvect2 size);

        static const unsigned int pixelWidth;
        static const unsigned int pixelHeight;

        static glm::mat4 projection;
        static LShaders::Shader* symbShader;
        static LShaders::Shader* textRectShader;
        static Character characters[CHAR_MAX + 1];
        static GLuint VAO, VBO;
        static LGraphics::LApp* app;

        static const size_t texturesPoolSize = 100;
        static LIRectangle* textRect[texturesPoolSize];
        static unsigned int stringFBO[texturesPoolSize], stringTexture[texturesPoolSize];

        //static bool refreshText[3];

        static size_t lastTextureNum;
    };
}
