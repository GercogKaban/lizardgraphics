#include "LRectangleMirror.h"
#include "LResourceManager.h"
#include "LApp.h"
#include "LBuffer.h"
#include "include/glm/gtc/type_ptr.hpp"

LGraphics::LRectangleMirror::LRectangleMirror(LApp* app)
	:LCube(app,nullptr)
{
    //LResourceManager::textures["skybox1"].textures
    //if (app->info.api == L_OPENGL)
    //    textures = std::make_shared<GLuint>(GET_DIFFUSE("skybox1"));
    //else if(app->info.api == L_VULKAN)
    //    textures = (void*)GET_DIFFUSE_VK("skybox1").texture;
	shader = app->skyBoxMirrorShader.get();
}

void LGraphics::LRectangleMirror::draw()
{
    if (isHidden())
        return;

    auto shader = ((LShaders::OpenGLShader*)getShader())->getShaderProgram();
    //auto lightPos = app->getLightPos();

    ((LShaders::OpenGLShader*)getShader())->use();

    const auto model = calculateModelMatrix();
    const auto proj = app->getProjectionMatrix();
    const auto view = app->getViewMatrix();
    const auto cameraPos = app->getCameraPos();

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *(GLuint*)getTexture());
    glBindVertexArray((buffer->getVaoNum()));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, buffer->getIndCount(), GL_UNSIGNED_SHORT, 0);
}
