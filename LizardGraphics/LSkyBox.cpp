#include "LSkyBox.h"
#include "LResourceManager.h"
#include "LApp.h"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LSkyBoxBuffer::LSkyBoxBuffer(LApp* app)
    :LCubeBuffer(app)
{
    LOG_CALL
}

void LGraphics::LSkyBoxBuffer::genBuffers()
{
    LOG_CALL
        if (app->getAppInfo().api == L_OPENGL)
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            //glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, getVertSize(), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }
    //else if (app->info.api == L_VULKAN)
//{
//    app->createBuffer(vertices, vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, getVertSize());
//    app->createBuffer(ebo, indexBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, getIndSize());
//}
}

LGraphics::LSkyBox::LSkyBox(LApp* app, ImageSkyboxResource res)
    :LCube(app, {res.name,false,false})
{
    LOG_CALL
    //auto text = LResourceManager::loadImageSkyboxResource(res);`
    textures = std::vector<TexturesData>{ LResourceManager::loadImageSkyboxResource(res) };
    this->buffer = app->standartSkyBoxBuffer;
    this->shader = app->skyBoxShader.get();
    if (app->skybox)
        app->toDelete.push(app->skybox);
    app->skybox = this;
    app->toCreate.pop();
}

LGraphics::LSkyBox::~LSkyBox()
{
}

void LGraphics::LSkyBox::draw()
{
    LOG_CALL
    if (isHidden())
        return;

    auto shader = ((LShaders::OpenGLShader*)getShader())->getShaderProgram();
    //auto lightPos = app->getLightPos();

    ((LShaders::OpenGLShader*)getShader())->use();
    const auto proj = app->getProjectionMatrix();
    const auto view = glm::mat4(glm::mat3(app->getViewMatrix()));

    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform1f(glGetUniformLocation(shader, "skyboxDepth"), skyboxDepth);
    if (app->fog.isEnabled)
    {
        glUniform3f(glGetUniformLocation(shader, "fog.color"), app->fog.color.x, app->fog.color.y, app->fog.color.z);
        glUniform1f(glGetUniformLocation(shader, "fog.density"), app->fog.density);
        glUniform1i(glGetUniformLocation(shader, "fog.isEnabled"), true);
    }

    auto casted = getTextures()[0];
    glBindTexture(GL_TEXTURE_CUBE_MAP, (TO_GL(casted)).id);
    glBindVertexArray(buffer->getVaoNum());
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
