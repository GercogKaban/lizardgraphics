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

    const char world_v[] =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform bool isometric;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "vec4 temp = projection * view * model * vec4(position, 1.0f);\n"
        "gl_Position = temp;\n"
        "if (isometric) {\n"
        "gl_Position.x = temp.x - temp.y;\n"
        "gl_Position.y = (temp.x + temp.y)/2.0f;\n"
        "}\n"
        "TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
        "}\n";

    const char lightningShader_v[] =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "uniform bool isometric;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "vec3 normal = vec3(0.0f,0.0f,1.0f);\n"
        "vec4 temp = projection * view * model * vec4(position, 1.0f);\n"
        "gl_Position = temp;\n"
        "if (isometric) {\n"
        "gl_Position.x = temp.x - temp.y;\n"
        "gl_Position.y = (temp.x + temp.y)/2.0f;\n"
        "}\n"
        "TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
        "FragPos = vec3(model * vec4(position, 1.0f));\n"
        "Normal = mat3(transpose(inverse(model*view * projection))) * normal;"
        "}\n";

    const char lightningShader_f[] =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "in vec3 FragPos;\n"
        "in vec3 Normal;\n"
        "out vec4 color;\n"
        "uniform sampler2D ourTexture;\n"
        "uniform vec4 color_;\n"
        "uniform bool sampleTexture;\n"
        "uniform vec3 lightPos;\n"
        "void main()\n"
        "{\n"
        "vec3 norm = normalize(Normal);\n"
        "vec3 lightDir = normalize(lightPos - FragPos);\n"
        "float diff = max(dot(norm, lightDir), 0.0);\n"
        "vec3 diffuse = diff * vec3(1.0f,1.0f,1.0f);\n"
        "float ambientStrength = 0.25f;\n"
        "vec3 ambient = ambientStrength * vec3(1.0f,1.0f,1.0f);\n"
        "vec4 result = vec4(ambient + diffuse,1.0f) * color_;\n"
        "if (sampleTexture) color = texture(ourTexture, TexCoord)*result;\n"
        "else color = result;\n"
        "}\n\0";

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
        OpenGLShader(LGraphics::LApp* app, const char* v_shader, const char* f_shader);
        void initShaders(const char* v_shader, const char* f_shader);
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
    };
}