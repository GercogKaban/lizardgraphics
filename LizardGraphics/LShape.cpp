﻿#include "LShape.h"
#include "pch.h"
#include "LShape.h"
#include "LApp.h"

namespace LGraphics
{
    void LShape::color(const glm::vec3 val)
    {
        color_ = val;
        if (app->getAppInfo().api == L_VULKAN)
            updateUniforms();
    }

    void LShape::color(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        color_ = { (float)r / (float)UINT8_MAX, (float)g / UINT8_MAX,(float)b / UINT8_MAX };
        updateUniforms();
    }

    void LShape::transparency(const float val)
    {
        transparency_ = val;
        updateUniforms();
    }

    void LShape::scale(const glm::vec3 val)
    {
        LShape::scale(val.x, val.y, val.z);
    }

    void LShape::scale(const float x, const float y, const float z)
    {
        auto scaleDif = glm::vec3(x, y, z) - scale_;
        scaleWithoutAlign({ x,y,z });
        updateUniforms();
    }

    void LShape::scaleWithoutAlign(const glm::vec3 val)
    {
        scale_ = val;
    }

    void LShape::move(const glm::vec3 val)
    {
        LShape::move(val.x, val.y, val.z);
    }

    void LShape::move(const float x, const float y, const float z)
    {
        auto moveDif = glm::vec3(x,y,z) - move_;
        move_ = { x,y,z };
        updateUniforms();
    }

    void LShape::move(const size_t x, const size_t y)
    {
        auto coords = pointOnScreenToGlCoords(glm::vec2(app->getWindowSize()), { (float)x,(float)y });
        LShape::move(coords.x, coords.y, 0.0f);
    }   

    void LShape::move(const glm::vec<2,size_t> v)
    {
        auto coords = pointOnScreenToGlCoords(glm::vec2(app->getWindowSize()), { (float)v.x, (float)v.y });
        LShape::move(coords.x, coords.y, 0.0f);
    }

    float& LShape::getTransparencyRef()
    {
        updateUniforms();
        return transparency_;
    }

    glm::vec3& LShape::getColorRef()
    {
        updateUniforms();
        return color_;
    }

    glm::vec3& LShape::getScaleRef()
    {
        updateUniforms();
        return scale_;
    }

    glm::vec3& LShape::getMoveRef()
    {
        updateUniforms();
        return move_;
    }

    LShape::LShape(LApp* app, const char * path)
        : LWidget(app, path)
    {
        if (path) turnOffColor();
    }

    void LShape::setBuffer(LBuffer* buffer)
    {
        this->buffer = buffer;
    }

    void LShape::refreshModel()
    {
        model = calculateModelMatrix();
    }

    glm::mat4 LGraphics::LShape::calculateModelMatrix() const
    {
#if _DEBUG
#include "../Optimized/optimized.h"
        return _calculateModelMatrix(move_, rotate_, scale_);
#else
        glm::mat4 model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
        model_ *= rotate_;
        model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
        return model_;
#endif
    }

    glm::vec3 LShape::getMove() const
    {
        return move_;
    }

    // нужно проверить эту функцию
    void LShape::turnOffColor()
    {
        color_ = rgbToFloat(1, 1, 1);
    }

    LShape::~LShape()
    {
        //delete buffer;
        //delete shader;
    }
}