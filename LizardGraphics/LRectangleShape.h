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

        glm::vec3 getTopLeftCorner() const;
        glm::vec3 getTopRightCorner() const;
        glm::vec3 getBottomLeftCorner() const;
        glm::vec3 getBottomRightCorner() const;

    protected:

        void init(LApp* app, bool isInterfaceObj);
    };
}

