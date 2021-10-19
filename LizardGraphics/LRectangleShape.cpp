#include "pch.h"
#include "LRectangleShape.h"
#include "LRectangleBuffer.h"
#include "Lshaders.h"
#include "LApp.h"
#include "LLogger.h"
#include "LResourceManager.h"

namespace LGraphics
{
    LRectangleShape::LRectangleShape(LApp* app, ImageResource res)
        :LShape(app),LImage(res,app->info.api)
    {
        init(app);
        shader = app->getLightningShader().get();
        app->toCreate.push(this);
        diffusePath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string() + '/' +
            app->qualityDirectories[app->info.texturesQuality] + "/diffuse/";
        normalsPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string() + '/' +
            app->qualityDirectories[app->info.texturesQuality] + "/normal/";
        displacementPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string() + '/' +
            app->qualityDirectories[app->info.texturesQuality] + "/displacement/";
    }

    void LRectangleShape::init(LApp* app)
    {
        LOG_CALL
            this->app = app;
        buffer = app->standartRectBuffer;
        if (app->info.api == L_OPENGL)
            shader = app->openGLLightShader.get();
        else if (app->info.api == L_VULKAN)
            shader = app->lightShader.get();
    }

    // deprecated
    void LRectangleShape::draw()
    {
        //auto shader = (LShaders::OpenGLShader*)getShader();
        //shader->use();
        //if (isTextureTurnedOn()) 
        //    glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 1);
        //else 
        //    glUniform1i(glGetUniformLocation(shader->getShaderProgram(), "sampleTexture"), 0);
        //auto texture = LResourceManager::toGl((LResourceManager::TexturesData*)getTextures()).id;
        //glBindTexture(GL_TEXTURE_2D, *(GLuint*)getTextures());
        //glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "move"), move_.x, move_.y, move_.z);
        //glUniform3f(glGetUniformLocation(shader->getShaderProgram(), "scale"), scale_.x, scale_.y, scale_.z);
        //glUniform4f(glGetUniformLocation(shader->getShaderProgram(), "color_"), color_.x, color_.y, color_.z, transparency_);
        //glBindVertexArray(buffer->getVaoNum());
        //glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);
        //if (label.text.size())
        //    LLine::display(label);
        /*if (innerWidgets)
            for (auto& i : *innerWidgets)
                i->draw();*/
    }

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

