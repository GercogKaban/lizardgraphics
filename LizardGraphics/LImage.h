#pragma once
#include "LWidgetI.h"

namespace LGraphics
{
    class LImage : public LWidgetI
    {
    public:
        GLuint getTexture() const { return texture; }

        void bindTexture(const char* path) override;
        void bindTexture(const unsigned char* bytes, size_t size) override;

    protected:

        LImage(const char* path);
        LImage(const unsigned char* bytes, size_t size);

        GLuint texture;
        void bindTexture(unsigned char* bytes, int width, int height);
    };
}

