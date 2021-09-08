#include "LCube.h"
#include "LApp.h"
#include "LCubeBuffer.h"
#include "include/glm/gtc/type_ptr.hpp"

std::vector<LGraphics::LWidget::CubeUniforms> LGraphics::LCube::uniforms;

#ifdef EXPERIMENTAL_SET
std::set<LGraphics::LCube*> LGraphics::LCube::objChanged;
#else
std::vector<LGraphics::LCube*> LGraphics::LCube::objChanged;
#endif
bool LGraphics::LCube::needToResetBuffer = false;
GLuint LGraphics::LCube::vbo;

LGraphics::LCube::LCube(LApp* app, const char* path)
	:LWRectangle(app,path)
{
    LOG_CALL
        //id = app->primitives[L_CUBE].size();
        //app->primitives[L_CUBE].push_back(this);
    if (uniforms.size() == uniforms.capacity())
       needToResetBuffer = true;
    uniforms.push_back(CubeUniforms());
	setBuffer(app->standartCubeBuffer);
}

void LGraphics::LCube::draw()
{
    LOG_CALL
    if (isHidden())
        return;

    auto shader = (LShaders::OpenGLShader*)getShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();

    // unique for each object
    const auto model = calculateModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shaderProgram, "objId"), id);
    //

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));
    if (!app->drawingInShadow)
    {
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();

        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
    }

    //glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), 1000.0f, 3.0f, 200.0f);
    //glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    //glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    //glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *(GLuint*)getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->depthMap);
    glBindVertexArray(buffer->getVaoNum());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glBindVertexArray(0);
}

void LGraphics::LCube::drawInstanced()
{
    auto shader = (LShaders::OpenGLShader*)app->getStandartShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);
    updateBuffer();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->ttt);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->depthMap);
    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, uniforms.size());
}

void LGraphics::LCube::setModel(const glm::mat4& model)
{
    LShape::setModel(model);
    objChanged.push_back(this);
}

void LGraphics::LCube::scale(const float x, const float y, const float z)
{
    LShape::scale(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LCube::move(const float x, const float y, const float z)
{
    LShape::move(x, y, z);
    objChanged.push_back(this);
}

void LGraphics::LCube::rotateX(float angle)
{
    LWidget::rotateX(angle);
    objChanged.push_back(this);
}

void LGraphics::LCube::rotateY(float angle)
{
    LWidget::rotateY(angle);
    objChanged.push_back(this);
}

void LGraphics::LCube::rotateZ(float angle)
{
    LWidget::rotateZ(angle);
    objChanged.push_back(this);
}

void LGraphics::LCube::updateBuffer()
{
    if (objChanged.size())
    {
        if (needToResetBuffer)
        {
            resetInstanceBuffer();
            needToResetBuffer = false;
        }
        LWidget::CubeUniforms* buffer = (LWidget::CubeUniforms*)glMapNamedBufferRange(vbo, 0, sizeof(LWidget::CubeUniforms) * uniforms.size(), 
            GL_MAP_WRITE_BIT);

#ifdef PARALLEL_UPDATE

        if (objChanged.size() > 1000 && app->info.freeThreads > 1)
        {
                std::thread* threads = new std::thread[app->info.freeThreads];
                for (size_t i = 0; i < app->info.freeThreads; ++i)
                {
                    const size_t interval = i * objChanged.size() / app->info.freeThreads;
                    threads[i] = std::thread(LCube::updateBufferParallel, buffer, objChanged, interval, interval + objChanged.size() / app->info.freeThreads);
                }

                for (size_t i = 0; i < app->info.freeThreads; ++i)
                    threads[i].join();
                delete[] threads;
        }
        else
#endif
        for (auto obj : objChanged)
            buffer[obj->id].model = ((LCube*)app->primitives[L_CUBE][obj->id])->calculateModelMatrix();
        objChanged.clear();
        glUnmapNamedBuffer(vbo);
    }
}

void LGraphics::LCube::setGlobalUniforms(GLuint shaderProgram)
{
    const auto proj = app->getProjectionMatrix();
    const auto view = app->getViewMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));

    if (!app->drawingInShadow)
    {
        glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), app->lightPos.x, app->lightPos.y, app->lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "dirPos"), 7.5f, 0.0f, 7.5f);
    }
}

void LGraphics::LCube::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LCube::resetInstanceBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::WidgetUniforms) * uniforms.capacity(), uniforms.data(), GL_STATIC_DRAW);

    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    const size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LGraphics::LCube::updateBufferParallel(LWidget::CubeUniforms* buffer, std::vector<LCube*>& changed, size_t begin, size_t end)
{
    for (size_t i = begin; i < end;i++)
        buffer[changed[i]->id].model = ((LCube*)app->primitives[L_CUBE][changed[i]->id])->calculateModelMatrix();
}
