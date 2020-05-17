#pragma once

#include <vector>

#include "LSymbol.h"

#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#include "Lshaders.h"
#include "LIRectangle.h"

#include "vectors.h"

namespace LGraphics
{
    class LWidgetI;
    class LBuffer;

    namespace
    {
    }

    class LApp : public LObject
    {
        friend LIRectangle;
        friend LRectangleShape;
        friend LShape;

    public:
        LApp();
        ~LApp(){releaseResources();}

        void loop();
        fvect2 getWindowSize() const { return { (float)width, (float)height }; }

        GLFWwindow* getWindowHandler() { return window; }

    private:

        void addObject(LWidgetI* w);

        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        //end = std::chrono::system_clock::now();

        void init(const int width, const int height);
        void initLEngine();
        void initOpenGl();

        void checkEvents();

        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        GLFWwindow* window;
        std::vector<LWidgetI*> objects;

        size_t width, height;

        //LBuffer* baseRectangleBuffer;

        //static LBuffer* baseRectangleTextBuffer;
    };
}

