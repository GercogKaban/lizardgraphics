#include "LShape.h"
#include "pch.h"
#include "LShape.h"
#include "LApp.h"

namespace LGraphics
{
    void LShape::color(const glm::vec3 val)
    {
        color_ = val;
        if (app->getAppInfo().api == L_VULKAN)
            setUpdateUniformsFlag();
    }

    void LShape::color(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        color_ = { (float)r / (float)UINT8_MAX, (float)g / UINT8_MAX,(float)b / UINT8_MAX };
        setUpdateUniformsFlag();
    }

    void LShape::transparency(const float val)
    {
        transparency_ = val;
        setUpdateUniformsFlag();
    }

    void LShape::scale(const glm::vec3 val)
    {
        LShape::scale(val.x, val.y, val.z);
    }

    void LShape::scale(const float x, const float y, const float z)
    {
        auto scaleDif = glm::vec3(x, y, z) - scale_;
        scaleWithoutAlign({ x,y,z });
        setUpdateUniformsFlag();
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
        setUpdateUniformsFlag();
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

    void LShape::setModel(const glm::mat4& model)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(model, scale_, rotate_q, move_, skew, perspective);
    }

    float& LShape::getTransparencyRef()
    {
        setUpdateUniformsFlag();
        return transparency_;
    }

    glm::vec3& LShape::getColorRef()
    {
        setUpdateUniformsFlag();
        return color_;
    }

    glm::vec3& LShape::getScaleRef()
    {
        setUpdateUniformsFlag();
        return scale_;
    }

    glm::vec3& LShape::getMoveRef()
    {
        setUpdateUniformsFlag();
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
        return _calculateModelMatrix(move_, glm::mat4_cast(rotate_q), scale_);
#else
        glm::mat4 model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
        model_ *= glm::mat4_cast(rotate_q);
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

    void LGraphics::LShape::setGlobalUniforms(GLuint shaderProgram)
    {
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));

        if (!app->drawingInShadow)
        {
            glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
            glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
            glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
        }
    }

    LShape::~LShape()
    {
        //delete buffer;
        //delete shader;
    }
}