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
    }

    void LBuffer::init()
    {
        setBuffers();
    }

    LBuffer::LBuffer()
    {
        if (vertices) delete[] vertices;
        if (indices) delete[] indices;
    }

    void LBuffer::genBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndSize(), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, coordsCount, GL_FLOAT, GL_FALSE, (coordsCount + textureCoordsCount) * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, textureCoordsCount, GL_FLOAT, GL_FALSE, (coordsCount + textureCoordsCount) * sizeof(GLfloat), (GLvoid*)(coordsCount * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    LBuffer::~LBuffer()
    {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        delete[] vertices;
        delete[] indices;
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
        vertices = new GLfloat[(coordsCount + textureCoordsCount)* verticesCount]
            {
                 0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
                 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
                -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
                -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
            };
    }

    void LRectangleBuffer::setInds()
    {
        indices = new GLuint[indicesCount]
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
        //!! такая функция уже есть в модуле structures.h !!
        //!! когда будет рефакторинг нужно будет переместить её в отдельный модуль !!
        // проверка на принадлежность точки четырехугольнику
        auto pointAttachment =[](int npol, float * xp, float * yp, fvect2 p)
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
        };

        double x, y;
        glfwGetCursorPos(app->getWindowHandler(), &x, &y);


        return false;
    }

    void LRectangleShape::draw()
    {
        auto shader = getShader();
        shader->use();
        glBindTexture(GL_TEXTURE_2D, getTexture());
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        glBindVertexArray(buffer->getVao());
        glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void LShape::setColor(const fvect3 val)
    {
        color_ = { (float)val.x / (float)sizeof(unsigned char), (float)val.y / (float)sizeof(unsigned char),(float)val.z / (float)sizeof(unsigned char) };
    }

    void LShape::setColor(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        color_ = { (float)r / (float)sizeof(unsigned char), (float)g / (float)sizeof(unsigned char),(float)b / (float)sizeof(unsigned char) };
    }

    void LShape::setTransparency(const float val)
    {
        transparency_ = val;
    }

    void LShape::setScale(const fvect3 val)
    {
        scale_ = val;
    }

    void LShape::move(const fvect3 val)
    {
        move_ = val;
    }

    LRectangleShape::LRectangleShape(LObject* parent , const char* path, LBuffer* buffer)
        :LShape(path,buffer){}

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

    void LApp::addObject(LWidgetI* shape)
    {
        shape->setApp(this);
        objects.push_back(shape);
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

        //glfwSetKeyCallback(window, key_callback);

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
    }

    void LApp::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
    {
        //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        //{
        //    double xpos, ypos;
        //    glfwGetCursorPos(window, &xpos, &ypos);
        //}
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

    LIRectangle::LIRectangle(LObject* parent, const char * path, LBuffer* buffer)
        :LRectangleShape(parent,path,buffer)
    {
        setShader(interfaceShader);
    }

    LIButton::LIButton(LObject * parent, const char * path, LBuffer *)
        :LRectangleShape(parent, path, buffer)
    {
        setShader(interfaceShader);
    }

    //LWRectangle::LWRectangle(LRectangleShape * rectangle)
    //    : LRectangleShapeI(rectangle)
    //{
    //    rectangle->setShader(sceneObjectShader);
    //}
}
