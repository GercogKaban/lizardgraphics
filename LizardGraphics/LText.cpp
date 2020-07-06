﻿#pragma once

#include "LText.h"
#include "pch.h"

#include <map>
#include <iostream>
#include <ft2build.h>
#include "include/GLEW/glew.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "LApp.h"
#include "Lshaders.h"

#include FT_FREETYPE_H 

namespace LGraphics
{
    LShaders::Shader* LText::shader;
    LText::Character LText::characters[CHAR_MAX+1];
    unsigned int LText::VAO, LText::VBO;
    LGraphics::LApp* LText::app;

    LText::LText(LGraphics::LApp* app)
    {
        this->app = app;
        shader = new LShaders::Shader(LShaders::symbol_v, LShaders::symbol_f);
        glm::mat4 projection = glm::ortho(0.0f, (float)app->getWindowSize().x, 0.0f, (float)app->getWindowSize().y);
        shader->use();
        glUniformMatrix4fv(glGetUniformLocation(shader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            throw std::exception("ERROR::FREETYPE: Could not init FreeType Library");

        FT_Face face;
        if (FT_New_Face(ft, "fonts\\OpenSans-Regular.ttf", 0, &face))
            throw std::exception("ERROR::FREETYPE: Failed to load font");

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (unsigned char c = 0; c < CHAR_MAX + 1; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                throw std::exception("ERROR::FREETYTPE: Failed to load Glyph");
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character =
            {
                texture,
                szvect2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                szvect2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            characters[c] = character;
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void LText::display(const std::string text, float x, float y, const float scale, const fvect3 color)
    {
        y = app->getWindowSize().y - y;
        getShader()->use();
        glUniform3f(glGetUniformLocation(getShader()->getShaderProgram(), "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
 
        for (const unsigned int const c: text)
        {
            const Character ch = characters[c];
            const float xpos = x + ch.bearing.x * scale;
            const float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            const float w = ch.size.x * scale;
            const float h = ch.size.y * scale;
            // update VBO for each character
            const float vertices[6][4] = 
            {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
        
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LText::display(Text text)
    {
        display(text.text, text.pos.x, text.pos.y, text.scale, text.color);
    }
}
