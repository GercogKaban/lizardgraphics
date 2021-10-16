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

LGraphics::LCube::LCube(LApp* app, ImageResource res)
	:LRectangleShape(app, res)
{
    LOG_CALL
    objChanged.push_back(this);
    if (uniforms.size() == uniforms.capacity())
       needToResetBuffer = true;
    uniforms.push_back(CubeUniforms());
	setBuffer(app->standartCubeBuffer);
    diffusePath = std::filesystem::current_path().generic_string() + '/' + app->texturesDirectories[app->info.texturesQuality] + "/diffuse/";
    normalsPath = std::filesystem::current_path().generic_string() + '/' + app->texturesDirectories[app->info.texturesQuality] + "/normal/";
    //shader = app->getLightningShader().get();
    //projection = app->getProjectionMatrix();
    //app->toCreate.push(this);
}

// deprecated
void LGraphics::LCube::draw()
{
    //LOG_CALL
    //if (isHidden())
    //    return;

    //auto shader = (LShaders::OpenGLShader*)getShader();
    //if (app->drawingInShadow)
    //    shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    //GLuint shaderProgram = shader->getShaderProgram();
    //shader->use();

    //// unique for each object
    //const auto model = calculateModelMatrix();
    //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glUniform1i(glGetUniformLocation(shaderProgram, "objId"), id);
    //setGlobalUniforms(shaderProgram);

    ////glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), 1000.0f, 3.0f, 200.0f);
    ////glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    ////glUniform3f(glGetUniformLocation(shader, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    ////glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
    //
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, *(GLuint*)getTexture());
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
    //glBindVertexArray(buffer->getVaoNum());
    //glDrawArrays(GL_TRIANGLES, 0, 36);
}

void LGraphics::LCube::drawInstanced()
{
    if (!uniforms.size()) return;
    auto shader = (LShaders::OpenGLShader*)app->getStandartShader();
    if (app->drawingInShadow)
        shader = ((LShaders::OpenGLShader*)app->shadowMapShader.get());
    GLuint shaderProgram = shader->getShaderProgram();
    shader->use();
    setGlobalUniforms(shaderProgram);
    updateBuffer();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    glDrawArraysInstanced(GL_TRIANGLES, 0, app->standartCubeBuffer->getVertices().size(), uniforms.size());
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

void LGraphics::LCube::setDiffuse(const char* path)
{
    imageResourceName = path;
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
        for (const auto& obj : objChanged)
            updateUniforms(buffer, obj->id);
        objChanged.clear();
        glUnmapNamedBuffer(vbo);
    }
}

void LGraphics::LCube::updateUniforms(LWidget::CubeUniforms* buffer, size_t id)
{
    auto path = ((LCube*)app->primitives[L_CUBE][id])->getDiffusePath();
    const auto subtexture = app->megatexture.subtextures[path];
    auto& obj = buffer[id];
    obj.model = ((LCube*)app->primitives[L_CUBE][id])->calculateModelMatrix();
    obj.offset = subtexture.first;
    obj.textureSize = subtexture.second;
}

void LGraphics::LCube::initInstanceBuffer()
{
    glGenBuffers(1, &vbo);
}

void LGraphics::LCube::resetInstanceBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::CubeUniforms) * uniforms.capacity(), uniforms.data(), GL_STATIC_DRAW);

    glBindVertexArray(app->standartCubeBuffer->getVaoNum());
    const size_t vec4Size = sizeof(glm::vec4);
    const size_t vec2Size = sizeof(glm::vec2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)(3 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)(4 * vec4Size));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(LWidget::CubeUniforms), (void*)((4 * vec4Size) + vec2Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LGraphics::LCube::updateBufferParallel(LWidget::RectangleUniforms* buffer, std::vector<LCube*>& changed, size_t begin, size_t end)
{
    for (size_t i = begin; i < end; i++)
        updateUniforms(buffer, changed[i]->id);
}
