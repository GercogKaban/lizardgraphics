#pragma once

#include <string>
#include <fstream>
#include <iostream>

#ifdef OPENGL
#include "include/GLEW/glew.h"
#endif

//#include "LApp.h"

namespace LGraphics
{
    class LApp;
}

namespace LShaders
{
    const char interface_v[] =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform vec3 move;\n"
        "uniform vec3 scale;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(position.x, position.y, position.z, 1.0)*vec4(scale,1.0) + vec4(move,0.0);\n"
        "TexCoord = vec2(texCoord.x, 1.0f + texCoord.y);\n"
        "}\0";

    //const char interface_vProj[] =
    //    "#version 330 core\n"
    //    "layout (location = 0) in vec3 position;\n"
    //    "layout (location = 1) in vec2 texCoord;\n"
    //    "out vec2 TexCoord;\n"
    //    "void main()\n"
    //    "{\n"
    //    "gl_Position = vec4(position.x, position.y, position.z, 1.0) * vec4(2.0f,2.0f,1.0f,1.0f);\n"
    //    "TexCoord = vec2(texCoord.x, 1.0f + texCoord.y);\n"
    //    "}\0";

    const char interface_f[] =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        //"layout(binding = 2, std430) buffer buffer1\n"
        //"{\n"
        //"int data_SSBO;\n"
        //"};\n"
        "uniform sampler2D ourTexture;\n"
        "uniform vec4 color_;\n"
        //"uniform vec2 mouse;\n"
        "uniform bool sampleTexture;\n"
        "void main()\n"
        "{\n"
        //"if ((pow(mouse.x - gl_FragCoord.x,2) + pow(mouse.y - gl_FragCoord.y,2)) <= 1.0) data_SSBO = 1;\n"
        "if (sampleTexture) color = texture(ourTexture, TexCoord)*color_;\n"
        "else color = color_;\n"
        "}\n\0";

    const char symbol_v[] =
        "#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n"
        "out vec2 TexCoords;\n"
        "uniform vec2 move_;\n"
        "uniform vec2 scale_;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(vertex.xy * scale_ + move_, 0.0, 1.0);\n"
        "TexCoords = vertex.zw;\n"
        "}\n";

    const char symbol_f[] =
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main()\n"
        "{\n"
        "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "color = vec4(textColor, 1.0) * sampled;\n"
        "}\n";

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

    const char checkMark_f[] =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D ourTexture;\n"
        "uniform vec4 color_;\n"
        "uniform bool sampleTexture;\n"
        "void main()\n"
        "{\n"
        "if (sampleTexture) color = texture(ourTexture, TexCoord)*color_;\n"
        "else color = color_;\n"
        "if (color.a == 0.0) color.rgba = vec4(color_.rgb, 1.0f);\n"
        "}\n\0";

    const char colorBar_f[] =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        "uniform vec4 firstColor;\n"
        "uniform vec4 secondColor;\n"
        "uniform int border;\n"
        "void main()\n"
        "{\n"
        "if (gl_FragCoord.x < border) color = secondColor; \n"
        "else color = firstColor;"
        "}\n\0";


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

    //const char lightningShader_v[] =
    //    "#version 330 core\n"
    //    "layout (location = 0) in vec3 position;\n"
    //    "layout (location = 1) in vec2 texCoord;\n"
    //    "out vec2 TexCoord;\n"
    //    "out vec3 eyeCordFs;\n"
    //    "out vec3 eyeNormalFs;\n"
    //    "uniform bool isometric;\n"
    //    "uniform mat4 model;\n"
    //    "uniform mat4 view;\n"
    //    "uniform mat4 projection;\n"
    //    "void main()\n"
    //    "{\n"
    //    "mat4 modelView = view*model;\n"
    //    "vec4 temp = projection * modelView * vec4(position, 1.0f);\n"
    //    "gl_Position = temp;\n"
    //    "if (isometric) {\n"
    //    "gl_Position.x = temp.x - temp.y;\n"
    //    "gl_Position.y = (temp.x + temp.y)/2.0f;\n"
    //    "}\n"
    //    "TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
    //    "vec3 normal = vec3(0.0f,0.0f,1.0f);\n"
    //    "mat4 normalMatrix = transpose(inverse(model*view));\n"
    //    "eyeCordFs = vec3(model*view * vec4(position, 1.0f));\n"
    //    "eyeNormalFs = normalize(vec3(normalMatrix * vec4(normal,0.0f)));\n"
    //    "}\n";

    //const char lightningShader_f[] =
    //    "#version 330 core\n"
    //    "in vec2 TexCoord;\n"
    //    "in vec3 eyeCordFs;\n"
    //    "in vec3 eyeNormalFs;\n"
    //    "out vec4 color;\n"
    //    "uniform sampler2D ourTexture;\n"
    //    "uniform vec4 color_;\n"
    //    "uniform bool sampleTexture;\n"
    //    "uniform vec3 lightPos;\n"
    //    "void main()\n"
    //    "{\n"
    //    "vec3 lightDir = normalize(lightPos - eyeCordFs);\n"
    //    "float diff = max(dot(eyeNormalFs, lightDir), 0.0);\n"
    //    "vec3 diffuse = diff * vec3(1.0f,1.0f,1.0f);\n"
    //    "float ambientStrength = 0.25f;\n"
    //    "vec3 ambient = ambientStrength * vec3(1.0f,1.0f,1.0f);\n"
    //    "vec4 result = vec4(ambient + diffuse,1.0f) * color_;\n"
    //    "if (sampleTexture) color = texture(ourTexture, TexCoord)*result;\n"
    //    "else color = result;\n"
    //    "}\n\0";

    /*!
    @brief Класс шейдера.

    Создаёт вершинный и фрагментный шейдер.
    */
    class Shader
    {
#ifdef OPENGL
        GLuint program;
#endif OPENGL

#ifdef VULKAN

        LGraphics::LApp* app;

        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkShaderModule genShaderModule(const char* shader, size_t size);

    public:

        VkPipeline getGraphicsPipeline() const {return graphicsPipeline;}
        VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
        //void destroy();

        //VkShaderModule& getVertShader() { return vertShader; }
        //VkShaderModule& getFragShader() { return fragShader; }

#endif

    public:

        ~Shader();
        Shader() {}

        void loadShaders(const char* vertShader, const char* fragShader);
        void initShaders(const char* v_shader, const char* f_shader, size_t vSize, size_t fSize);

        /*!
        @brief Конструктор

        Создаёт вершинный и фрагментный шейдер.
        @param v_shader - массив вершинного шейдера
        @param v_shader - массив фрагментного шейдера
        */
        Shader(const GLchar* v_shader, const GLchar* f_shader, LGraphics::LApp* app, bool sourceCode = true);
        void bindShader(const GLchar* shader, short shaderType);

        /*!
        @brief Активирует шейдер (перед отрисовкой).
        */
        void use() const
        {
#ifdef OPENGL
            glUseProgram(program);
#endif OPENGL
        }

        /*!
        @brief Возвращает шейдерную программу.
        */
#ifdef OPENGL
        GLuint getShaderProgram() const
        {
            return program;
        }
#endif OPENGL

    protected:
        const char* loadShader(const char* shader, size_t& shaderSize);
    };
}