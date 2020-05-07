#include "pch.h"
#include "LizardGraphics.h"
#include "textures.h"

namespace LGraphics
{
    std::vector<std::string>LError::errors;
    size_t LError::lastErrorNum;
    LShaders::Shader* interfaceShader = nullptr;
    LShaders::Shader* sceneObjectShader = nullptr;

    namespace
    {
        LBuffer* baseRectangleBuffer = new LRectangleBuffer();
        LTextBuffer* baseRectangleTextBuffer = new LTextBuffer();
    }

    void LBuffer::init()
    {
        setBuffers();
    }

    LBuffer::LBuffer()
    {
        if (textures) delete[] textures;
        if (vertices) delete[] vertices;
        if (ebo) delete[] ebo;
    }

    void LBuffer::genBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(2, VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndSize(), ebo, GL_STATIC_DRAW);

        glVertexAttribPointer(0, coordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, getTextureSize(), textures, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(1, textureCoordsCount, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    LBuffer::~LBuffer()
    {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(2, VBO);
        glDeleteVertexArrays(1, &VAO);
        delete[] vertices;
        delete[] ebo;
        delete[] textures;
    }


    LImage::LImage(const char* path)
    {
        bindTexture(path);
    }

    LImage::LImage(const unsigned char* bytes, size_t size)
    {
        bindTexture(bytes, size);
    }

    void LImage::bindTexture(const char* path)
    {
        int width, height;
        unsigned char* image;

        if (!path)
            image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
        else
        {
            image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
            if (!image)
            {
                image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
                LWRITE("item at address: ", path, " is missing"), LPRINT();
            }
        }
        bindTexture(image, width, height);
    }

    void LImage::bindTexture(const unsigned char* bytes,size_t size)
    {
        int width, height;
        auto image = SOIL_load_image_from_memory(bytes, size, &width, &height, 0, SOIL_LOAD_RGBA);
        bindTexture(image, width, height);
    }

    void LImage::bindTexture(unsigned char* bytes, int width, int height)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LRectangleBuffer::setBuffers()
    {
        verticesCount = 4; indicesCount = 6, textureCoordsCount = 2;

        setVerts();
        setInds();
        genBuffers();
    }

    void LRectangleBuffer::setVerts()
    {
        vertices = new GLfloat[coordsCount * verticesCount]
        {
             0.5f,  0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f, 
            -0.5f,  0.5f, 0.0f,
        };

        textures = new GLfloat[textureCoordsCount* verticesCount]
        {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };
    }

    void LRectangleBuffer::setInds()
    {
        ebo = new GLuint[indicesCount]
        {
            0,1,3,
            1,2,3
        };
    }

    std::list<LWidgetI*> LRectangleShape::intersectsWith()
    {
        return std::list<LWidgetI*>();
    }

    bool LRectangleShape::mouseOnIt()
    {
        float x_[4], y_[4];
        auto vbo = buffer->getVertices();

        // getting rectangle coordinates 
        for (size_t i = 0; i < buffer->getVerticesCount(); ++i)
        {
            x_[i] = (vbo[i*3] *getScale().x) + getMove().x;
            y_[i] = (vbo[i*3 + 1] * getScale().y) + getMove().y;
        }

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->getWindowHandler(), &mouse_x, &mouse_y);
        return isPointInPolygon(buffer->getVerticesCount(), x_, y_, pointOnDisplayToGlCoords(app->getWindowSize(), { (float)mouse_x ,(float)mouse_y }));
    }

    void LRectangleShape::draw()
    {
        if (dynamic_cast<LText*>(this))
            dynamic_cast<LText*>(this)->setTextureBuffer();
        auto shader = getShader();
        shader->use();
        glBindTexture(GL_TEXTURE_2D, getTexture());
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        glBindVertexArray(buffer->getVaoNum());
        glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

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
        scale_ = val;
    }

    void LShape::scale(const float x, const float y, const float z)
    {
        scale_ = { x,y,z };
    }

    void LShape::move(const fvect3 val)
    {
        move_ = val;
    }

    void LShape::move(const float x, const float y, const float z)
    {
        move_ = { x,y,z };
    }

    void LShape::move(const size_t x, const size_t y)
    {
        auto coords = pointOnDisplayToGlCoords(app->getWindowSize(), { (float)x,(float)y });
        move_ = { coords.x,coords.y, 0.0f };
    }

    void LShape::move(const szvect2 v)
    {
        auto coords = pointOnDisplayToGlCoords(app->getWindowSize(), fvect2((float)v.x,(float)v.y));
        move_ = { coords.x,coords.y, 0.0f };
    }

    LRectangleShape::LRectangleShape(LApp* app, LObject* parent , const char* path, LBuffer* buffer)
        :LShape(path,buffer)
    {
        this->app = app;
        app->addObject(this);
    }

    LRectangleShape::LRectangleShape(LApp * app, LObject * parent, const unsigned char * bytes, size_t size, LBuffer* buffer)
        :LShape(bytes,size, buffer)
    {
        this->app = app;
        app->addObject(this);
    }

    LApp::LApp()
    {
        init(1920, 1080);
    }

    void LApp::loop()
    {

        while (!glfwWindowShouldClose(window))
        {

            glfwPollEvents();
            checkEvents();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for (auto& o : objects)
                if (!o->isHidden())
                    o->draw();
            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }

    void LApp::addObject(LWidgetI * w)
    {
        objects.push_back(w);
    }

    void LApp::init(const int width, const int height)
    {
        this->width = width, this->height = height;
        initOpenGl();
        initLEngine();
    }

    void LApp::initLEngine()
    {
        LError::init();
        interfaceShader = new LShaders::Shader(LShaders::interface_v, LShaders::interface_f);
        baseRectangleBuffer->init();
        baseRectangleTextBuffer->init();
        
        //auto background = new LIRectangle();
        //background->setScale({ 2.0f,2.0f,1.0f });
        //background->bindTexture(notexture, notextureSize);
        //addObject(background);
    }

    void LApp::initOpenGl()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(width, height, "window", nullptr, nullptr);
        glfwMakeContextCurrent(window);

 
        LApp* myWindow = this;
        glfwSetWindowUserPointer(window, myWindow);

        auto func = [](GLFWwindow* w, int button, int action , int mods )
        {
            static_cast<LApp*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
        };

        glfwSetMouseButtonCallback(window, func);

        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        int width_, height_;
        glfwGetFramebufferSize(window, &width_, &height_);
        glViewport(0, 0, width_, height_);
    }

    void LApp::checkEvents()
    {
        //for (auto& o : objects)
        //{
        //    o.
        //}
    }

    void LApp::releaseResources()
    {
        for (auto& x : objects)
            delete x;
        LError::releaseResources();

        delete baseRectangleBuffer;
        delete baseRectangleTextBuffer;
    }

    void LApp::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            for (auto& o : objects)
            {
                if (dynamic_cast<LIButton*>(o))
                    if (o->mouseOnIt())
                        dynamic_cast<LIButton*>(o)->doClickEventFunction();
            }
        }
    }

    void LShape::setShader(LShaders::Shader* shader)
    {
        this->shader = shader;
    }

    LShape::LShape(const char* path, LBuffer * buffer)
        :LImage(path)
    {
        initBuffer(buffer);
    }

    LShape::LShape(const unsigned char* bytes, size_t size, LBuffer* buffer)
        : LImage(bytes,size)
    {
        initBuffer(buffer);
    }

    void LShape::initBuffer(LBuffer* buffer)
    {
        if (!buffer) this->buffer = baseRectangleBuffer;
        else this->buffer = buffer;
    }

    LIRectangle::LIRectangle(LApp* app,LObject* parent, const char * path, LBuffer* buffer)
        :LRectangleShape(app,parent,path,buffer)
    {
        setShader(interfaceShader);
    }

    //LWRectangle::LWRectangle(LRectangleShape * rectangle)
//    : LRectangleShapeI(rectangle)
//{
//    rectangle->setShader(sceneObjectShader);
//}

    LIButton::LIButton(LApp* app, LObject * parent, const char * path, LBuffer * buffer)
        :LRectangleShape(app,parent, path, buffer)
    {
        setShader(interfaceShader);
    }

    void LIButton::setClickEventFunction(std::function<void()> fun)
    {
        clickFunction = fun;
    }

    void LIButton::doClickEventFunction()
    {
        clickFunction();
    }

    bool isPointInPolygon(int npol, float* xp, float* yp, fvect2 p)
    {
        bool c = false;
        for (int i = 0, j = npol - 1; i < npol; j = i++)
        {
            if ((
                (yp[i] < yp[j]) && (yp[i] <= p.y) && (p.y <= yp[j]) &&
                ((yp[j] - yp[i]) * (p.x - xp[i]) > (xp[j] - xp[i]) * (p.y - yp[i]))
                ) || (
                (yp[i] > yp[j]) && (yp[j] <= p.y) && (p.y <= yp[i]) &&
                    ((yp[j] - yp[i]) * (p.x - xp[i]) < (xp[j] - xp[i]) * (p.y - yp[i]))
                    ))
                c = !c;
        }
        return c;
    }


    fvect2 pointOnDisplayToGlCoords(fvect2 displaySize, fvect2 point)
    {
        point.y = displaySize.y - point.y;
        return point / displaySize - (fvect2(1.0f, 1.0f) - point / displaySize);
    }

    LTextBuffer::LTextBuffer()
    {
        const szvect2 charSize = ivect2(14, 24);
        const size_t charIndex_to = 127;
        const size_t charsInLine = 83;
        const szvect2 offset = ivect2(1 + charSize.x * (charIndex_from - 30), 4);

        const szvect2 textureSize = { 1180,79 };
        fvect2 textureSizeF = textureSize.cut<fvect2>();


        textCoords = new GLfloat*[charIndex_to - charIndex_from];
        for (size_t i = 0; i < charIndex_to - charIndex_from; ++i)
            textCoords[i] = new GLfloat[8];


        for (int i = charIndex_from; i < charIndex_to; i++)
        {
            int x = offset.x;
            int y = offset.y;

            x += charSize.x * (i - charIndex_from);
            while (x >= charsInLine * charSize.x)
                x -= charsInLine * charSize.x, y += charSize.y;

            // здесь должна быть формула
            GLfloat textCoords_[] =
            {
                (x + charSize.x)/ textureSizeF.x,   1.0f - y/ textureSizeF.y,
                (x + charSize.x) / textureSizeF.x,  1.0f - (y + charSize.y) / textureSizeF.y,
                x / textureSizeF.x ,                  1.0f - (y + charSize.y) / textureSizeF.y,
                x/ textureSizeF.x ,                   1.0f - y/ textureSizeF.y                   // верхний левый угол
            };

            memcpy(textCoords[i - charIndex_from], textCoords_, 8 * sizeof(GLfloat));

            //14 х 24

            // 1.0f, 1.0f, x + symbolSize y
            // 1.0f, 0.0f, x + symbolSize.x y 
            // 0.0f, 0.0f, 
            // 0.0f, 1.0f  x y
        }

        //vbo = new GLfloat[coordsCount* verticesCount];
        //{
        //     0.5f,  0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,   y / textureSize.y,
        //     0.5f, -0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,  (y - symbolSize.y) / textureSize.y,
        //    -0.5f, -0.5f, 0.0f,  x / textureSize.x,                   (y - symbolSize.y) / textureSize.y,
        //    -0.5f,  0.5f, 0.0f,  x / textureSize.x,                    y / textureSize.y
        //};
        //vbo = new GLfloat[(coordsCount + textureCoordsCount)* verticesCount]
        //{
        //     0.5f,  0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,   y / textureSize.y,
        //     0.5f, -0.5f, 0.0f,  (x + symbolSize.x) / textureSize.x,  (y - symbolSize.y) / textureSize.y,
        //    -0.5f, -0.5f, 0.0f,  x / textureSize.x,                   (y - symbolSize.y) / textureSize.y,
        //    -0.5f,  0.5f, 0.0f,  x / textureSize.x,                    y / textureSize.y
        //};
    }

    LText::LText(LApp* app_)
        :LRectangleShape(app_,nullptr, new_font,new_fontSize, baseRectangleTextBuffer)
    {
        setShader(interfaceShader);
    }

    void LText::setTextureBuffer()
    {
       // вся текстура
       //GLfloat bf_[] =
       //{
       //     1.0f, 1.0f,
       //     1.0f, 0.0f,
       //     0.0f, 0.0f,
       //     0.0f, 1.0f
       //};

        glBindVertexArray(buffer->getVaoNum());
        auto b = dynamic_cast<LTextBuffer*>(buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->getVBO()[1]);
        // пока что будет выводиться (нет) только 1 символ
        glBufferData(GL_ARRAY_BUFFER, buffer->getTextureSize(), b->textCoords['f' - b->charIndex_from] , GL_STREAM_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
}