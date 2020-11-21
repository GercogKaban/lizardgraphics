#include "LTextRender.h"
#pragma once

#include "pch.h"
#include "LTextRender.h"

#include "LApp.h"

#include FT_FREETYPE_H 

namespace LGraphics
{
    LShaders::Shader* LTextRender::symbShader;
    LTextRender::Character LTextRender::characters[CHAR_MAX+1];
    unsigned int LTextRender::VAO, LTextRender::VBO;
    LGraphics::LApp* LTextRender::app;
    unsigned int LTextRender::stringFBO[texturesPoolSize], LTextRender::stringTexture[texturesPoolSize];
    LIRectangle* LTextRender::textRect[texturesPoolSize];
    LShaders::Shader* LTextRender::textRectShader;
    glm::mat4 LTextRender::projection;
    const size_t LTextRender::texturesPoolSize; 
    size_t LTextRender::lastTextureNum = 0;
    const unsigned int LTextRender::pixelWidth = 0;
    const unsigned int LTextRender::pixelHeight = 48;
    
#define half  0.5f
    const float vertices[6][4] =
    {
        { -half, half,   0.0f, 0.0f },
        { -half, -half,   0.0f, 1.0f },
        { half, -half,   1.0f, 1.0f },

        { -half, half,   0.0f, 0.0f },
        { half, -half,   1.0f, 1.0f },
        { half, half,   1.0f, 0.0f }
    };
#undef half

    //bool LTextRender::refreshText[3] = { false,false,false };

    LTextRender::LTextRender(LGraphics::LApp* app)
    {
        this->app = app;
        symbShader = new LShaders::Shader(LShaders::symbol_v, LShaders::symbol_f);
        projection = glm::ortho(0.0f, (float)app->getWindowSize().x, 0.0f, (float)app->getWindowSize().y);
       //textRectShader = new LShaders::Shader(LShaders::interface_vProj, LShaders::interface_f);

        //glm::mat4 rotate_ = glm::mat4(1.0f);
        //rotate_ = glm::rotate(rotate_, glm::radians(180.0f), { 1.0f,0.0f,0.0f });
        //rotate_ = glm::scale(rotate_, glm::vec3(2.0f, 2.0f, 1.0f));

        for (size_t i = 0; i < texturesPoolSize; ++i)
        {
            textRect[i] = new LIRectangle(app);
            //textRect[i]->scale(2.0f, 2.0f, 1.0f);
            textRect[i]->setShader(app->standartInterfaceshader);
            //textRect[i]->getShader()->use();
            textRect[i]->turnOnTexture();
            textRect[i]->color(255, 255, 255);
            //textRect[i]->projection_ = projection;
            //textRect[i]->setRotate(rotate_);
            app->removeWidget(textRect[i]);
            //glUniformMatrix4fv(glGetUniformLocation(textRect[i]->getShader()->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        }
      
        symbShader->use();
        glUniformMatrix4fv(glGetUniformLocation(symbShader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            throw std::exception("ERROR::FREETYPE: Could not init FreeType Library");

        FT_Face face;
        if (FT_New_Face(ft, "fonts\\OpenSans-Regular.ttf", 0, &face))
            throw std::exception("ERROR::FREETYPE: Failed to load font");

        FT_Set_Pixel_Sizes(face, pixelWidth, pixelHeight);

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glGenFramebuffers(texturesPoolSize, stringFBO);
        glGenTextures(texturesPoolSize, stringTexture);
        //for (size_t i = 0; i < texturesPoolSize; ++i)
        {
        //    genTexture(i, { 1024, 256 });
        }
        //    glBindFramebuffer(GL_FRAMEBUFFER, stringFBO[i]);
        //    glBindTexture(GL_TEXTURE_2D, stringTexture[i]);
        //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->getWindowSize().x, app->getWindowSize().y, 0, GL_RGBA, GL_FLOAT, NULL);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        //    glBindTexture(GL_TEXTURE_2D, 0);
        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stringTexture[i], 0);
        //    glReadBuffer(GL_NONE);
        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //    textRect[i]->setTexture(stringTexture[i]);
        //}
    }

    LTextRender::~LTextRender()
    {
        delete symbShader;
        for (auto& t : textRect)
            delete t;
    }
    
    void LTextRender::display(const std::string& text, fvect2 pos, float scale, const fvect3 color, const szvect2 screenOrTextureSize)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, stringFBO);
        //pos.y = app->getWindowSize().y - pos.y;

        //scale *= 10;

        getShader()->use();
        glUniform3f(glGetUniformLocation(getShader()->getShaderProgram(), "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        for (const unsigned int c : text)
        {
            const Character ch = characters[c];
            float xpos = pos.x + ch.bearing.x * scale;
            float ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            // update VBO for each character

            xpos /= screenOrTextureSize.x;
            ypos /= screenOrTextureSize.y;

            //вообще-то - рендерится ли на экране
            if (screenOrTextureSize.x > 1000)
            {
                xpos -= 0.5f;
                ypos -= 0.5f;
                xpos *= 2;
                ypos *= 2;
                ypos = -ypos;
            }

            w /= screenOrTextureSize.x;
            h /= screenOrTextureSize.y;
            xpos += w / 2;
            ypos += h / 2;

            glUniform2f(glGetUniformLocation(getShader()->getShaderProgram(), "move_"), xpos, ypos);
            glUniform2f(glGetUniformLocation(getShader()->getShaderProgram(), "scale_"), w, h);

            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // update content of VBO memory
            //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            //glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            //glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            pos.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        //glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D, 0);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //glBindTexture(GL_TEXTURE_2D, 0);
    }

    //void LTextRender::display(Text& text)
    //{
    //    fvect2 pos = glCoordsToScreenCoords(app->getWindowSize(), { text.pos.x,text.pos.y});
    //    display(text.text, pos.x, app->getWindowSize().y - pos.y, text.scale, text.color);
    //}
    //
    //void LTextRender::displayText(const std::vector<Text>& textObjects, int refreshMode)
    //{
    //    if (refreshText[refreshMode])
    //    {
    //        glBindFramebuffer(GL_FRAMEBUFFER, stringFBO[refreshMode]);
    //        for (auto& t : textObjects)
    //            display(t.text, t.pos.x, t.pos.y, t.scale, t.color);
    //        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //        refreshText[refreshMode] = false;
    //    }
    //    if (textObjects.size())
    //        textRect[refreshMode]->draw();
    //}

    void LTextRender::displayText()
    {
        glActiveTexture(GL_TEXTURE0);

        for (size_t i = 0; i < app->textObjects.size(); ++i)   
        {
            assert(i < texturesPoolSize && "Textures container is full! PLEASE DO POOL");
            auto& t = app->textObjects[i];
            if (!t->haveRefreshed())
            {
                auto textLength = LTextRender::getTextLength(*t);
                auto size = fvect2{ t->getPos().x + textLength - t->getPos().x , (float)pixelHeight };

                // нужно приспособить для примитивов разных размеров
                textRect[i]->scale(fvect3{ textLength / (float)(app->getWindowSize().x / 4.0f), (float)pixelHeight / (float)(app->getWindowSize().y / 4.0f), 1.0f });
                textRect[i]->move(xOnScreenToGlCoord(app->getWindowSize(), (t->getPos().x + t->getPos().x + textLength) / 2.0f),
                    yOnScreenToGlCoord(app->getWindowSize(), t->getPos().y), 0.0f);

                genTexture(i, size);
                glViewport(0, 0, size.x, size.y);
                glBindFramebuffer(GL_FRAMEBUFFER, stringFBO[i]);

                //glClear(GL_COLOR_BUFFER_BIT);

                display(t->getText(), fvect2(0), t->getScale(), t->getColor(), size);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0, 0, app->getWindowSize().x, app->getWindowSize().y);
                t->refreshed = true;
            }

            textRect[i]->setTexture(stringTexture[i]); //

            //glBindTexture(GL_TEXTURE0, stringTexture[i]);
            textRect[i]->draw();
        }
    }

    void LTextRender::addText(Text* text)
    {
        app->textObjects.push_back(text);
    }

    float LTextRender::getTextLength(const Text& text)
    {
        float res = 0.0f;
        for (auto& s : text.getText())
            res += (characters[s].advance >> 6) * text.getScale();
        res -= (characters[text.getText().back()].advance >> 6) * text.getScale();
        return res;
    }

    void LGraphics::LTextRender::genTexture(int num, fvect2 size)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, stringFBO[num]);
        glBindTexture(GL_TEXTURE_2D, stringTexture[num]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stringTexture[num], 0);

        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        textRect[num]->setTexture(stringTexture[num]);
    }
}
