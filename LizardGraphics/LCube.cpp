#include "LCube.h"
#include "LApp.h"
#include "LCubeBuffer.h"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LCube::LCube(LApp* app, const char* path)
	:LWRectangle(app,path)
{
    LOG_CALL
    //auto texture = (GLuint*)getTexture();
    //app->cubeInstantPool.uniformBuffers[id].diffuseSampler = texture? *texture : UINT_MAX;
    //app->cubeInstantPool.uniformBuffers[id].id = id;
    app->cubeInstantPool.uniformBuffers[id].model = calculateModelMatrix();
    //app->cubeInstantPool.uniformBuffers[id].shadowMapSampler = app->depthMap;
    app->cubeInstantPool.objs[id] = this;
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

    const auto proj = app->getProjectionMatrix();
    const auto view = app->getViewMatrix();
    const auto model = calculateModelMatrix();

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(app->lightSpaceMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);
    glUniform1i(glGetUniformLocation(shaderProgram, "objId"), id);

    if (!app->drawingInShadow)
    {
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
