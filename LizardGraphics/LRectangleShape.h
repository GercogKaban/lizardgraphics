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

        glm::mat4 projection_ = glm::mat4(1.0f);
        const char* getObjectType() override { return "LRectanlgeShape"; }

        bool mouseOnIt() override; ///< Возвращает находится ли мышка на виджете.
#ifdef VULKAN
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum);
#endif
#ifdef OPENGL
        void draw() override; ///< Рисует виджет на сцене.

        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param bytes - массив байт (rgba).
        @param size - размер массива bytes.
        @param component - указатель на LBaseComponent.
        */
        LRectangleShape(LApp* app, const unsigned char* bytes = nullptr, size_t size = 0, bool isInterfaceObj = true);
#endif
        //LRectangleShape(LApp* app);

        /*!
        @brief Конструктор.
        @param app - указатель на приложение LApp.
        @param parent - родительский виджет (пока не работает).
        @param path - путь к текстуре.
        @param component - указатель на LBaseComponent.
        */
        LRectangleShape(LApp* app, const char* path = nullptr, bool isInterfaceObj = true);

        float calculateWidgetLength();

        fvect3 getCenter() const override;

        fvect3 getTopLeftCorner() const;
        fvect3 getTopRightCorner() const;
        fvect3 getBottomLeftCorner() const;
        fvect3 getBottomRightCorner() const;

    protected:

        void init(LApp* app, bool isInterfaceObj);
    };
}

