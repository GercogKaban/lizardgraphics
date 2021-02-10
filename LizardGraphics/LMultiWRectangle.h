#pragma once

#include "LWRectangle.h"
#include "Lshaders.h"
#include "LApp.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LApp;
    class LNonWidget : public LObject
    {
    public:
        virtual void draw() = 0;

        bool isInterface;

        LApp* app;

        LShaders::Shader* shader;

        LNonWidget(LApp* app) : app(app)
        {

        }
    };

    //using T = LWidget;
    template <typename T>
    class LWidgetGroup : public LNonWidget
    {
    public:
        std::vector<T*> widgets;

        LWidgetGroup(LApp* app) : LNonWidget(app)
        {

        }

        void addWidget(T* widget)
        {
            widgets.push_back(widget);
            app->removeWidget(widget);
        }

        virtual void clear()
        {
            for (auto* w : widgets)
                delete w;
            widgets.clear();
        }

#ifdef OPENGL
        virtual void draw() override
        {
            for (auto* w : widgets)
            {
                w->setShader(shader);
                w->draw();
            }
        }
#endif
    };

    class LMultiWRectangle : public LWidgetGroup<LWRectangle>
    {
    public:

        std::vector<uint32_t> textures;
        //std::vector<LWRectangle*> rectangles;
        std::vector<glm::mat4> models;

        const char* getObjectType() const override { return "LMultiWRectangle"; }
        LMultiWRectangle(LApp* app, size_t rectCount);
        //bool isInterfaceObject() const override { return false; }

        //virtual glm::vec2 getScreenCoords(size_t _i) const { return rectangles[_i].getScreenCoords(); }
        //void setMatrices(size_t _i, LApp* app) { rectangles[_i].setMatrices(app); }
        //void setMatrices(size_t _i) { rectangles[_i].setMatrices(); }
        void draw() override;

        size_t rectCount;

        //glm::mat4 calculateModelMatrix(size_t _i) const { rectangles[_i].calculateModelMatrix(); }
        //glm::mat4 getModelMatric(size_t _i) const { return calculateModelMatrix(_i); }

    private:

        LBuffer* buffer;

        //LShaders::Shader* shader;

        //glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

}

