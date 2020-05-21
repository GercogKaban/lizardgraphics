#pragma once

#include "LObject.h"
#include "include/GLEW/glew.h"

namespace LGraphics
{
    class LBuffer : public LObject
    {
    public:

        const GLuint* getVBO() const { return VBO; }
        const GLuint getVaoNum() const { return VAO; }
        const auto getVertices() const { return vertices; }
        size_t getVertSize() const { return verticesCount * coordsCount * sizeof(GLfloat); }
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
}

