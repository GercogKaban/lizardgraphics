#pragma once

#include <functional>

//#include "LApp.h"
#include "vectors.h"
#include "Lshaders.h"
#include "LObject.h"

namespace LGraphics
{
    class LApp;

    class LWidgetI : public LObject
    {
    public:

        void setApp(LApp* app) { this->app = app; }

        virtual void draw() = 0;

        virtual void transparency(const float val) = 0;

        virtual void color(const unsigned char r, const unsigned char g, const unsigned char b) = 0;
        virtual void color(const fvect3 val) = 0;

        virtual void scale(const fvect3 val) = 0;
        virtual void scale(const float x, const float y, const float z) = 0;

        virtual void move(const fvect3 val) = 0;
        virtual void move(const float x, const float y, const float z) = 0;
        virtual void move(const size_t x, const size_t y) = 0;
        virtual void move(const szvect2 v) = 0;

        void setMouseOnItEventFunction(std::function<void()> fun) { mouseOnItFunction = fun; }

        virtual float getTransparency() const = 0;
        virtual fvect3 getColor() const = 0;
        virtual fvect3 getScale() const = 0;
        virtual fvect3 getMove() const = 0;

        virtual void bindTexture(const char* path) = 0;
        virtual void bindTexture(const unsigned char* bytes, size_t size) = 0;

        virtual void setShader(LShaders::Shader* shader) = 0;

        //virtual std::list<LWidgetI*> intersectsWith() = 0;
        virtual bool mouseOnIt() = 0;

        void show() { isHidden_ = false; }
        void hide() { isHidden_ = true; }
        void setHidden(bool hide) { isHidden_ = hide; }
        bool isHidden() const { return isHidden_; }

        virtual ~LWidgetI() {}

    protected:

        LApp* app = nullptr;
        bool isHidden_ = false;
        std::function<void()> mouseOnItFunction;

        LWidgetI(){}
    };
}
