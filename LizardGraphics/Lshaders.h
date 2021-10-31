#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "vulkan/vulkan.h"
#include "include/GLEW/glew.h"

//#include "LApp.h"

namespace LGraphics
{
    class LApp;
}

namespace LShaders
{
    /*!
    @brief Класс шейдера.

    Создаёт вершинный и фрагментный шейдер.
    */
    class Shader
    {
    protected:
        LGraphics::LApp* app;

    public:

        Shader(LGraphics::LApp* app) { this->app = app; }
        virtual ~Shader() {}
        virtual const char* getObjectType() const { return "Shader"; }

    protected:
        const char* loadShader(const char* shader, size_t& shaderSize);
    };

    class VulkanShader : public Shader
    {
    public:

        const char* getObjectType() const override { return "VShader"; }
        VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }
        VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

        VulkanShader(LGraphics::LApp* app, const char* vertPath, const char* fragPath);
        ~VulkanShader();

        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkShaderModule genShaderModule(const char* shader, size_t size);
        void initShaders(const char* v_shader, const char* f_shader, size_t vSize, size_t fSize);
    };

    class OpenGLShader : public Shader
    {
    public:

        const char* getObjectType() const override { return "OGLShader"; }
        OpenGLShader(LGraphics::LApp* app, const char* vShader, const char* fShader, const char* tessControlShader,
            const char* tessEvalShader);
        void initShaders(const char* vShader, const char* fShader, const char* tessControlShader,
            const char* tessEvalShader);
        /*!
        @brief Возвращает шейдерную программу.
        */
        GLuint getShaderProgram() const
        {
            return program;
        }

        /*!
        @brief Активирует шейдер (перед отрисовкой).
        */
        void use() const
        {
            glUseProgram(program);
        }
    private:
        GLuint program;

        void checkCreation(GLuint shader, const std::string& shaderType) const;
        GLuint compileShader(const GLchar* source, int shaderType) const;
    };
}