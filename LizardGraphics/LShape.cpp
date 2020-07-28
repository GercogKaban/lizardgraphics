#include "LShape.h"
#include "LBaseComponent.h"
#include "LApp.h"
#include "pch.h"
#include "additional.h"

namespace LGraphics
{
    void LShape::color(const fvect3 val)
    {
        color_ = { (float)val.x / (float)sizeof(unsigned char), (float)val.y / (float)sizeof(unsigned char),(float)val.z / (float)sizeof(unsigned char) };
    }

    void LShape::color(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        color_ = { (float)r / (float)sizeof(unsigned char), (float)g / (float)sizeof(unsigned char),(float)b / (float)sizeof(unsigned char) };
    }

    void LShape::transparency(const float val)
    {
        transparency_ = val;
    }

    void LShape::scale(const fvect3 val)
    {
        alignLabel();
        scale_ = val;
    }

    void LShape::scale(const float x, const float y, const float z)
    {
        alignLabel();
        scaleWithoutAlign({ x,y,z });
    }

    void LShape::scaleWithoutAlign(const fvect3 val)
    {
        scale_ = val;
    }

    void LShape::move(const fvect3 val)
    {
        move(val.x, val.y, val.z);
    }

    void LShape::move(const float x, const float y, const float z)
    {
        auto moveDif = fvect3(x,y,z) - move_;
        move_ = { x,y,z };
        for (auto& o : innerWidgets)
            o->move(o->getMove() + moveDif);
    }

    void LShape::move(const size_t x, const size_t y)
    {
        auto coords = pointOnScreenToGlCoords(fvect2(app->getWindowSize()), { (float)x,(float)y });
        move(coords.x, coords.y, 0.0f);
    }   

    void LShape::move(const szvect2 v)
    {
        auto coords = pointOnScreenToGlCoords(fvect2(app->getWindowSize()), { (float)v.x, (float)v.y });
        move(coords.x, coords.y, 0.0f);
    }

    void LShape::setShader(LShaders::Shader* shader)
    {
        if (!shader) throw std::exception("error, no shader\n");
        this->shader = shader;
    }

    LShape::LShape(const char* path, LBaseComponent* component)
        :LWidgetI(path)
    {
        if (!component->getBuffer()) throw std::exception("error, no buffer\n");
        this->buffer = component->getBuffer();
    }

    LShape::LShape(const unsigned char* bytes, size_t size, LBaseComponent* component)
        : LWidgetI(bytes, size)
    {
        if (!component->getBuffer()) throw std::exception("error, no buffer\n");
        this->buffer = component->getBuffer();
    }

    void LShape::setBuffer(LBuffer* buffer)
    {
        this->buffer = buffer;
    }

    void LShape::turnOffColor()
    {
        color_ = rgbToFloat(1, 1, 1);
    }

    void LShape::setLabelColor(unsigned char r, unsigned char g, unsigned char b)
    {
        label.color = rgbToFloat(r,g,b);
    }

    void LShape::setLabel(const std::string label)
    {
    }

    //void LShape::updateLabelPos()
    //{
    //    this->label.pos = { coords.x + labelTextStartPosition, coords.y };
    //}
}