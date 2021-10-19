#pragma once
#include "LShape.h"
#include "LImage.h"

namespace LGraphics
{
    class LApp;
    /*!
    @brief Класс четурёхугольной формы.
    */
    class LImagedShape : public LShape, public LImage
    {
    public:

        const char* getObjectType() const override { return "LRectanlgeShape"; }

        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum) {}
        void draw() {}
        LImagedShape(LApp* app, ImageResource res, std::vector<LShape*>& changed, std::vector<LGraphics::LWidget::PrimitiveUniforms>& u,
            bool& needToResetBuffer);

        const LBuffer* getBuffer() const { return buffer; }
        void setBuffer(LBuffer* buffer) { this->buffer = buffer; }

        void setModel(std::vector<LShape*>& changed, LShape* shape, const glm::mat4& model);
        void scale(std::vector<LShape*>& changed, LShape* shape, const float x, const float y, const float z);
        void move(std::vector<LShape*>& changed, LShape* shape, const float x, const float y, const float z);
        void rotateX(std::vector<LShape*>& changed, LShape* shape, float angle);
        void rotateY(std::vector<LShape*>& changed, LShape* shape, float angle);
        void rotateZ(std::vector<LShape*>& changed, LShape* shape, float angle);

    protected:

        static void updateBufferParallel(LGClasses type, LWidget::PrimitiveUniforms* buffer, std::vector<LShape*>& changed, size_t begin, size_t end);
        static void updateBuffer(LGClasses type, std::vector<LGraphics::LShape*>& changed, bool needToResetBuffer
            , GLuint vao, GLuint vbo, std::vector<LWidget::PrimitiveUniforms>& u);
        static void drawInstanced(LGClasses type, std::vector<LShape*>& changed, bool needToResetBuffer, GLuint vao,
            GLuint vbo, std::vector<LWidget::PrimitiveUniforms>& uniforms, size_t indCount);
        static void resetInstanceBuffer(GLuint vao, GLuint vbo, const std::vector<LWidget::PrimitiveUniforms> uniforms);
        static void updateUniforms(LApp* app, LGClasses primitive, LWidget::PrimitiveUniforms* buffer, size_t id);

        void init(LApp* app);
        LBuffer* buffer = nullptr; ///< Координатный буфер (вершины, индексы, текстурные координаты, нормали).
    };
}

