#pragma once

#include "LBuffer.h"
#include "LImage.h"

#include "vectors.h"

namespace LGraphics
{
    class LBaseComponent;
    class LShape : public LImage
    {
    public:

        void setShader(LShaders::Shader* shader) override;

        void color(const fvect3 val) override;
        void color(const unsigned char r, const unsigned char g, const unsigned char b) override;
        void transparency(const float val) override;

        void scale(const fvect3 val) override;
        void scale(const float x, const float y, const float z) override;

        void move(const fvect3 val) override;
        void move(const float x, const float y, const float z) override;
        void move(const size_t x, const size_t y) override;
        void move(const szvect2 v) override;

        float getTransparency() const override { return transparency_; }
        fvect3 getColor() const override { return color_; }
        fvect3 getScale() const override { return scale_; }
        fvect3 getMove() const override { return move_; }

        virtual ~LShape()
        {
            //if (buffer)delete buffer;
        }

    protected:

        LShape(const char* path = nullptr, LBaseComponent* buffer = nullptr);
        LShape(const unsigned char* bytes, size_t size, LBaseComponent* buffer = nullptr);

        void initBuffer(LBuffer* buffer);

        const LShaders::Shader* getShader() const { return shader; }

        fvect3 scale_ = fvect3(0.5f, 0.5f, 1.0f);
        fvect3 move_ = fvect3(0.0f, 0.0f, 0.0f);
        fvect3 color_ = fvect3(1.0f, 1.0f, 1.0f);
        float transparency_ = 1.0f;

        LShaders::Shader* shader;
        LBuffer* buffer;
    };
}

