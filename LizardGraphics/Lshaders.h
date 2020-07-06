#pragma once

#include <string>
#include "include/GLEW/glew.h"

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
        "TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
        "}\0";

    const char interface_f[] =
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
        "}\n\0";
    
    const char symbol_v[] =
        "#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n"
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "gl_Position = projection* vec4(vertex.xy, 0.0, 1.0);\n"
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

    /*!
    @brief Класс шейдера.

    Создаёт вершинный и фрагментный шейдер.
    */
    class Shader
    {
        GLuint program;
    public:

        /*!
        @brief Конструктор

        Создаёт вершинный и фрагментный шейдер.
        @param v_shader - массив вершинного шейдера
        @param v_shader - массив фрагментного шейдера
        */
        Shader(const GLchar* v_shader, const GLchar* f_shader)
        {
            GLuint vertex, fragment;
            GLint success;
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &v_shader, NULL);
            glCompileShader(vertex);
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success)
                throw std::exception("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");

            // Fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &f_shader, NULL);
            glCompileShader(fragment);

            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success)
                throw std::exception("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");

            // Shader Program
            this->program = glCreateProgram();
            glAttachShader(this->program, vertex);
            glAttachShader(this->program, fragment);
            glLinkProgram(this->program);

            glGetProgramiv(this->program, GL_LINK_STATUS, &success);
            if (!success)
                throw std::exception("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");

            glDeleteShader(vertex);
            glDeleteShader(fragment);

        }

        /*!
        @brief Активирует шейдер (перед отрисовкой).
        */
        void use() const
        {
            glUseProgram(program);
        }

        /*!
        @brief Возвращает шейдерную программу.
        */
        GLuint getShaderProgram() const
        {
            return program;
        }
    };
}