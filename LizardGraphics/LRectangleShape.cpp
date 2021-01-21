#include "pch.h"
#include "LRectangleShape.h"
#include "LRectangleBuffer.h"
#include "Lshaders.h"
#include "LApp.h"

namespace LGraphics
{
    LRectangleShape::LRectangleShape(LApp* app, const char* path, bool isInterfaceObj)
        :LShape(app,path)
    {
        init(app, isInterfaceObj);
    }

#ifdef OPENGL
    LRectangleShape::LRectangleShape(LApp* app, const unsigned char * bytes, size_t size, bool isInterfaceObj)
        :LShape(app,bytes,size)
    {
        init(app, isInterfaceObj);
    }
#endif 

    float LRectangleShape::calculateWidgetLength()
    {
        return xGlCoordToScreenCoord(app->getWindowSize(), getTopRightCorner().x) - xGlCoordToScreenCoord(app->getWindowSize(), getTopLeftCorner().x);
    }

    fvect3 LRectangleShape::getCenter() const
    {
        return ((((LRectangleBuffer*)buffer)->getTopLeftCorner() * scale_ + move_) + (((LRectangleBuffer*)buffer)->getBottomRightCorner() * scale_ + move_)) / 2.0f;
    }

    fvect3 LRectangleShape::getTopLeftCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getTopLeftCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getTopRightCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getTopRightCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getBottomLeftCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getBottomLeftCorner() * scale_ + move_;
    }

    fvect3 LRectangleShape::getBottomRightCorner() const
    {
        return ((LRectangleBuffer*)buffer)->getBottomRightCorner() * scale_ + move_;
    }

    void LRectangleShape::init(LApp* app, bool isInterfaceObj)
    {
        this->app = app;
        buffer = app->standartRectBuffer;
#ifdef OPENGL
        shader = app->standartInterfaceshader;
#endif
#ifdef VULKAN
        shader = app->baseShader;
#endif
        app->addObject(this);
    }

    bool LRectangleShape::mouseOnIt()
    {
        float x_[4], y_[4];
        float* vbo = buffer->getVertices();

        // getting rectangle coordinates 
        for (size_t i = 0; i < buffer->getVerticesCount(); ++i)
        {
            x_[i] = vbo[i * 3] * getScale().x + getMove().x;
            y_[i] = vbo[i * 3 + 1] * getScale().y + getMove().y;
        }

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->getWindowHandler(), &mouse_x, &mouse_y);

        return isPointInPolygon((int)buffer->getVerticesCount(), x_, y_, pointOnScreenToGlCoords(app->getWindowSize(), { (float)mouse_x ,(float)mouse_y }));
    }
#ifdef OPENGL
    void LRectangleShape::draw()
    {
        auto shader = getShader();
        shader->use();
        if (isTextureTurnedOn()) 
            glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 1);
        else 
            glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, getTexture());
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        //glUniformMatrix4fv(glGetUniformLocation(shader->getShaderProgram(), "rotate"), 1, GL_FALSE, glm::value_ptr(rotate_));

        glBindVertexArray(buffer->getVaoNum());
        glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);
        //if (label.text.size())
        //    LLine::display(label);
        if (innerWidgets)
            for (auto& i : *innerWidgets)
                i->draw();
    }
#endif OPENGL

    //LRectangleShape::LRectangleShape(LApp* app)
    //    :LShape()
    //{
    //    init(app);
    //}


    void LRectangleShape::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex, size_t objectNum)
    {
#ifdef VULKAN1
        auto shader = getShader(); 
        auto model = dynamic_cast<LWRectangle*>(this)->
        app->updateUniformBuffer(frameIndex, this);

        uint32_t dynamicOffset = objectNum * static_cast<uint32_t>(app->dynamicAlignment);

        LApp::TransformData data{ 
        {color_.x,color_.y,color_.z, transparency_}
        ,{move_.x, move_.y, move_.z}
        , {scale_.x, scale_.y, scale_.z } };
      
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getGraphicsPipeline());
        VkBuffer vertexBuffers[] = { buffer->getVertBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, buffer->getIndBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            getShader()->getPipelineLayout(), 0, 1, &app->descriptorSets[objectNum*2 + frameIndex], 1, &dynamicOffset);

        vkCmdPushConstants(commandBuffer, shader->getPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LApp::TransformData), &data);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(buffer->getIndCount()), 1, 0, 0, 0);
        //if (innerWidgets)
        //    for (auto& i : *innerWidgets)
        //        i->draw(commandBuffer, frameIndex);
#endif
    }
    //LRectangleShape::LRectangleShape(LApp* app)
    //    :LShape()
    //{
    //    init(app);
    //}
}

