#pragma once

#define GLEW_STATIC

#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <iostream>
#include <functional>
#include <chrono>
#include "Lshaders.h"
#include "include/GLFW/glfw3.h"

#include "vectors.h"

#define LWRITE(...) LError::writeError(__VA_ARGS__)
#define LPRINT(...) LError::printErrors()
#define LDISPLAY() LError::displayErrors()

namespace LGraphics
{
    // need moving into another module
    bool isPointInPolygon(int npol, float * xp, float * yp, fvect2 p);
    fvect2 pointOnDisplayToGlCoords(fvect2 displaySize, fvect2 point);

    class LBuffer;
    class LTextBuffer;
    class LApp;
    class LText;

    namespace
    {
        extern LBuffer* baseRectangleBuffer;
        extern LTextBuffer* baseRectangleTextBuffer;
    }

    class LObject
    {
    protected:
        LObject() {}
    };

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

        virtual std::list<LWidgetI*> intersectsWith() = 0;
        virtual bool mouseOnIt() = 0;

        void show() { isHidden_ = false; }
        void hide() { isHidden_ = true; }
        void setHidden(bool hide) { isHidden_ = hide; }
        bool isHidden() const { return isHidden_; }

        virtual ~LWidgetI(){}

    protected:
        
        LApp* app = nullptr;
        bool isHidden_ = false;
        std::function<void()> mouseOnItFunction;

        LWidgetI(){}
    };

    class LBuffer : public LObject
    {
    public:

        const GLuint* getVBO() const { return VBO; }
        const GLuint getVaoNum() const { return VAO; }
        const auto getVertices() const { return vertices; }
        size_t getVertSize() const { return verticesCount * coordsCount  * sizeof(GLfloat); }
        size_t getTextureSize() const { return textureCoordsCount * verticesCount * sizeof(GLfloat); }
        size_t getIndSize() const { return indicesCount * sizeof(GLuint); }
        size_t getIndCount() const { return indicesCount; }
        size_t getVerticesCount() const { return verticesCount; }

        void init();

        virtual ~LBuffer();

    protected:

        LBuffer();

        virtual void setBuffers() = 0;
        virtual void setVerts() = 0;
        virtual void setInds() = 0;

        void genBuffers();

        GLfloat* textures = nullptr;
        GLfloat* vertices = nullptr;
        GLuint* ebo = nullptr;
        GLuint VBO[2], VAO, EBO;

        size_t verticesCount, indicesCount, coordsCount = 3, textureCoordsCount = 0;

    };

    class LRectangleBuffer : public LBuffer
    {
    protected:
        void setBuffers() override;
        void setVerts() override;
        void setInds() override;
    };

    class LImage : public LWidgetI
    {
    public:
        GLuint getTexture() const { return texture; }

        void bindTexture(const char* path) override;
        void bindTexture(const unsigned char* bytes, size_t size) override;

    protected:

        LImage(const char* path);
        LImage(const unsigned char* bytes, size_t size);

        GLuint texture;
        void bindTexture(unsigned char* bytes, int width, int height);
    };

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

        LShape(const char* path = nullptr, LBuffer* buffer = nullptr);
        LShape(const unsigned char* bytes, size_t size, LBuffer* buffer = nullptr);

        void initBuffer(LBuffer* buffer);

        const LShaders::Shader* getShader() const { return shader; }

        fvect3 scale_ = fvect3(0.5f, 0.5f, 1.0f);
        fvect3 move_ = fvect3(0.0f, 0.0f, 0.0f);
        fvect3 color_ = fvect3(1.0f, 1.0f, 1.0f);
        float transparency_ = 1.0f;

        LShaders::Shader* shader;
        LBuffer* buffer;
    };

    class LRectangleShape : public LShape
    {
    public:

        std::list<LWidgetI*> intersectsWith() override;
        bool mouseOnIt() override;

        void draw() override;
        LRectangleShape(LApp* app,LObject* parent = nullptr, const char* path = nullptr, LBuffer* = nullptr);
        LRectangleShape(LApp* app, LObject* parent = nullptr, const unsigned char* bytes = nullptr, size_t size = 0, LBuffer* = nullptr);
    };

    class LIRectangle : public LRectangleShape
    {
    public:
        LIRectangle(LApp* app, LObject* parent = nullptr, const char* path = nullptr, LBuffer* = nullptr);
    };

    //class LWRectangle : public LRectangleShapeI
    //{
    //public:
    //    LWRectangle(LRectangleShape* rectangle);
    //};

    class LIButton : public LRectangleShape
    {
    public:

        friend LApp;

        LIButton(LApp* app,LObject* parent = nullptr, const char* path = nullptr, LBuffer* = nullptr);

        void setClickEventFunction(std::function<void()> fun);


    protected:

        std::function<void()> clickFunction;
        void doClickEventFunction();
    };

    //
    //class LTextEdit : public LRectangle
    //{
    //
    //};

    class LApp : public LObject
    {
        friend LRectangleShape;
    public:
        LApp();
        ~LApp()
        {
            releaseResources();
        }
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
    };

    class LError : public LObject
    {
    public:

        static void init()
        {
            errors.push_back("");
        }

        static void printToConsole(const std::string& error)
        {
            lastErrorNum++;
            std::cout << error << std::endl;
        }

        static void printToDisplay(const std::string& error)
        {
            // ...
        }

        template <typename T>
        static void writeError(T t)
        {
            errors.back() += t;
            errors.push_back("");
        }

        template<typename T, typename... Args>
        static void writeError(T t, Args... args)
        {
            errors.back() += t;
            writeError(args...);
        }

        static void printErrors()
        {
            std::for_each(errors.begin() + lastErrorNum, errors.end() - 1, printToConsole);
        }

        static void releaseResources()
        {
            errors.clear();
            lastErrorNum = 0;
        }

    protected:

        static std::vector<std::string> errors;
        static size_t lastErrorNum;
    };

    class LTextBuffer : public LRectangleBuffer
    {
        friend LText;
    public:
        LTextBuffer();
    protected:
        GLfloat** textCoords;
        const size_t charIndex_from = 32;
    };

    class LText : public LRectangleShape
    {
        friend LRectangleShape;
    public:
        LText(LApp* app);
    protected:
        // эта переменная не работает, передается мусор :(
        //const char fontPath[37] = "../lizardgraphics/res/font_white.png";
        void setTextureBuffer();
    };
}