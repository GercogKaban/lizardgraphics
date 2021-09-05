#pragma once
#include "LShape.h"

namespace LGraphics
{
    class LApp;

    /*!
    @brief Класс четурёхугольной формы.
    */
    class LRectangleShape : public LShape
    {
    public:

        const char* getObjectType() const override { return "LRectanlgeShape"; }

        bool mouseOnIt() override; ///< Возвращает находится ли мышка на виджете.
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum);
        void draw();
        LRectangleShape(LApp* app, const char* path = nullptr, bool isInterfaceObj = true);

        float calculateWidgetLength();

        glm::vec3 getTopLeftCorner() const;
        glm::vec3 getTopRightCorner() const;
        glm::vec3 getBottomLeftCorner() const;
        glm::vec3 getBottomRightCorner() const;

    protected:

        void init(LApp* app, bool isInterfaceObj);
    };
}

