#pragma once

#include "LWRectangle.h"
#include "Lshaders.h"

#include "include/glm/glm.hpp"

namespace LGraphics
{
    class LMultiWRectangle : public LRectangleShape
    {
    public:

        std::vector<GLint> textures;
        std::vector<LWRectangle> rectangles;
        std::vector<glm::mat4> models;

        const char* getObjectType() override { return "LMultiWRectangle"; }
        LMultiWRectangle(LApp* app, size_t rectCount);
        bool isInterfaceObject() const override { return false; }

        virtual glm::vec2 getScreenCoords(size_t _i) const { return rectangles[_i].getScreenCoords(); }
        void setMatrices(size_t _i, LApp* app) { rectangles[_i].setMatrices(app); }
        void setMatrices(size_t _i) { rectangles[_i].setMatrices(); }
        void draw() override;

        size_t rectCount;

        //glm::mat4 calculateModelMatrix(size_t _i) const { rectangles[_i].calculateModelMatrix(); }
        //glm::mat4 getModelMatric(size_t _i) const { return calculateModelMatrix(_i); }

    private:

        LBuffer* buffer;

        LShaders::Shader* shader;

        //glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

}

