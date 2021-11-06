#pragma once

#include <functional>
#include <vector>
#include <queue>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/quaternion.hpp"

//#include "vectors.h"
#include "Lshaders.h"
#include "LObject.h"

namespace LGraphics
{
    class LApp;
    class LImagedShape;
    class LPlane;
    class LCube;

    /*!
    @brief Абстрактный класс виджета. От него наследуются все виджеты Lizard Graphics.
    */
    class LWidget : public LObject
    {
    public:

        friend LImagedShape;
        friend LApp;
        friend LPlane;
        friend LCube;
        /*void setApp(LApp* app_) { this->app = app_; }  ///< Устанавливает приложение (окно) виджета.*/

        const char* getObjectType() const override { return "LWidget"; }
        virtual void tick() {}
        virtual void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex) = 0; ///< Рисует виджет.
        virtual void draw(){}

        enum UniformChanges
        {
            UNMODIFIED,
            ONE_BUFFER_TO_CHANGE,
            TWO_BUFFERS_TO_CHANGE,
            THREE_BUFFERS_TO_CHANGE,
        };
    
        struct PrimitiveUniforms
        {
            glm::mat4 model;
            glm::vec4 diffuseCoords;
            glm::vec4 normalCoords;
            glm::vec4 heightCoords;
            glm::vec4 reflexCoords;
            glm::ivec3 mapping;
            glm::mat3 inverseModel;
            glm::ivec2 ids;
        };

        //using PlaneUniforms = WidgetUniforms;
        //using CubeUniforms = WidgetUniforms;

        //struct WidgetUniformsRef
        //{
        //    glm::mat4& model;
        //    int& id;
        //    GLuint& diffuseSampler, &shadowMapSampler;
        //};

        /*!
        @brief Устанавливает цвет виджету.
        @param val - float [0;1].
        */
        virtual void transparency(const float val) = 0;

        /*!
        @brief Устанавливает цвет виджету.
        @param r - красный цвет, [0;255].
        @param g - зелёный цвет, [0;255].
        @param b - синий цвет, [0;255].
        */
        virtual void color(const unsigned char r, const unsigned char g, const unsigned char b) = 0;

        /*!
        @brief Устанавливает цвет виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        virtual void color(const glm::vec3 val) = 0;

        /*!
        @brief Устанавливает размеры виджету.
        @param x - x координата.
        @param y - y координата.
        @param z - z координата.
        */
        virtual void scale(const float x, const float y, const float z) = 0;

        /*!
        @brief Устанавливает размеры виджету.
        @param val - вектор, значения которого находятся в промежутке [0;1].
        */
        virtual void scale(const glm::vec3 val) = 0;


        /*!
        @brief Перемещает виджет.
        @param val - вектор.
        */
        virtual void move(const glm::vec3 val) = 0;

        /*!
        @brief Перемещает виджет.
        @param x - x координата на сцене.
        @param y - y координата на сцене.
        @param z - z координата на сцене.
        */
        virtual void move(const float x, const float y, const float z) = 0;

        /*!
        @brief Перемещает виджет.
        @param x - пиксель по x.
        @param y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        virtual void move(const size_t x, const size_t y) = 0;

        /*!
        @brief Перемещает виджет.
        @param val - вектор, где x - пиксель по x, y - пиксель по y.
        @warning x увеличивается вправо, y - вниз.
        */
        virtual void move(const glm::vec<2, size_t> v) = 0;

        virtual void setModel(const glm::mat4& model) = 0;

        virtual void setMaterial(int material) { this->material = material; setUpdateUniformsFlag();}
        virtual int getMaterial() const { return material; }

        void setName(::std::string name) { this->name = name; }
        ::std::string getName() const { return name; }

        virtual void rotateX(float angleDegree);
        virtual void rotateY(float angleDegree);
        virtual void rotateZ(float angleDegree);

        glm::vec3 getRotateDegrees() const { return rotateDegrees; }
        void setRotate(const glm::mat4& rotate);

        virtual void turnOffColor() = 0;

        virtual float getTransparency() const = 0;   ///< Возвращает прозрачность виджета.
        virtual glm::vec3 getColor() const = 0;         ///< Возвращает цвет виджета.
        virtual glm::vec3 getScale() const = 0;         ///< Возвращает размеры виджета.
        virtual glm::vec3 getMove() const  = 0;          ///< Возвращает вектор move виджета.

        virtual glm::quat getRotateQ() const { return rotate_q; }
        virtual glm::mat4 getRotate() const { return glm::mat4_cast(rotate_q); }

        virtual float& getTransparencyRef() = 0;   ///< Возвращает прозрачность виджета.
        virtual glm::vec3& getColorRef() = 0;         ///< Возвращает цвет виджета.
        virtual glm::vec3& getScaleRef() = 0;         ///< Возвращает размеры виджета.
        virtual glm::vec3& getMoveRef() = 0;          ///< Возвращает вектор move виджета.


        virtual glm::quat& getRotateRef() { return rotate_q; }
        virtual bool isInstanced() const { return false; }

        LShaders::Shader* getShader() { return shader; }
        void setShader(LShaders::Shader* s) { shader = s; }

        void show() { isHidden_ = false; }   ///< Показать виджет.
        void hide() { isHidden_ = true; }    ///< Скрыть виджет.
        void setHidden(bool hide) { isHidden_ = hide; }   ///< Установить видимость виджета.
        bool isHidden() const { return isHidden_; }       ///< Возвращает спрятан ли виджет.
        bool isInited() const { return isInited_; }
        bool isChanged() const { return changed != UNMODIFIED; }
        //virtual int getId() const { return id; }
        
        virtual ~LWidget();

    protected:

        std::pair<float, float> rangeX, rangeY, rangeZ;
        uint64_t id = 0;
        int material = 0;

        glm::quat rotate_q = glm::quat(0,0,0,1);
        //glm::mat4 rotate_ = glm::mat4(1.0f);
        glm::vec3 rotateDegrees = glm::vec3(0.0f);

        static LApp* app;    ///< Указатель на приложение.
        bool isHidden_ = false; ///< Видимость виджета.
        bool isInited_ = false;
        LShaders::Shader* shader = nullptr; ///< Шейдер.

        int changed = 3;
        ::std::string name;

        void setUpdateUniformsFlag();

        // временное название

        //virtual void setGlobalUniforms(GLuint shader) {}

        size_t arrayIndex = 0;

        /*!
        @brief Конструктор.
        @param path - Путь к изображению.
        */
        LWidget(LApp* app/*, ImageResource res*/);
    };
}
