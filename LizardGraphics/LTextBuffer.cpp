#include "LTextBuffer.h"
#include "pch.h"

namespace LGraphics
{
    LTextBuffer::LTextBuffer()
    {
        const szvect2 charSize = ivect2(14, 24);
        const size_t charIndex_to = 127;
        const size_t charsInLine = 83;
        const szvect2 offset = ivect2(1 + charSize.x * ((int)charIndex_from - 30), 4);

        const szvect2 textureSize = { 1180,79 };
        fvect2 textureSizeF = textureSize.cut<fvect2>();


        textCoords = new GLfloat*[charIndex_to - charIndex_from];
        for (size_t i = 0; i < charIndex_to - charIndex_from; ++i)
            textCoords[i] = new GLfloat[8];


        for (int i = charIndex_from; i < (int)charIndex_to; i++)
        {
            int x = offset.x;
            int y = offset.y;

            x += charSize.x * (i - charIndex_from);
            while (x >= charsInLine * charSize.x)
                x -= charsInLine * charSize.x, y += charSize.y;

            // здесь должна быть формула
            GLfloat textCoords_[] =
            {
                (x + charSize.x) / textureSizeF.x,   1.0f - y / textureSizeF.y,
                (x + charSize.x) / textureSizeF.x,  1.0f - (y + charSize.y) / textureSizeF.y,
                x / textureSizeF.x ,                  1.0f - (y + charSize.y) / textureSizeF.y,
                x / textureSizeF.x ,                   1.0f - y / textureSizeF.y                   // верхний левый угол
            };

            memcpy(textCoords[i - charIndex_from], textCoords_, 8 * sizeof(GLfloat));

            //14 х 24

            // 1.0f, 1.0f, x + symbolSize y
            // 1.0f, 0.0f, x + symbolSize.x y 
            // 0.0f, 0.0f, 
            // 0.0f, 1.0f  x y
        }

        //vbo = new GLfloat[coordsCount* verticesCount];
        //{
        //     0.5f,  0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,   y / textureSize.y,
        //     0.5f, -0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,  (y - symbolSize.y) / textureSize.y,
        //    -0.5f, -0.5f, 0.0f,  x / textureSize.x,                   (y - symbolSize.y) / textureSize.y,
        //    -0.5f,  0.5f, 0.0f,  x / textureSize.x,                    y / textureSize.y
        //};
        //vbo = new GLfloat[(coordsCount + textureCoordsCount)* verticesCount]
        //{
        //     0.5f,  0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,   y / textureSize.y,
        //     0.5f, -0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,  (y - symbolSize.y) / textureSize.y,
        //    -0.5f, -0.5f, 0.0f,  x / textureSize.x,                   (y - symbolSize.y) / textureSize.y,
        //    -0.5f,  0.5f, 0.0f,  x / textureSize.x,                    y / textureSize.y
        //};
    }
}