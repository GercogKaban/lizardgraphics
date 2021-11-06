#include "LPicking.h"
#include "LApp.h"

LGraphics::LPicking::LPicking(LApp* app, size_t size)
{
	this->size = size;

    LApp::FBOAttach depthAttachment, colorAttachment;
    depthAttachment.attachmentSize = size;
    depthAttachment.componentType = GL_DEPTH_COMPONENT;
    depthAttachment.textureType = GL_TEXTURE_2D;
    depthAttachment.valuesType = GL_FLOAT;

    colorAttachment = depthAttachment;
    colorAttachment.componentType = GL_RGB32UI;
    colorAttachment.valuesType = GL_UNSIGNED_INT;

    const auto depthAttach = app->createAttachment(depthAttachment);
    const auto colorAttach = app->createAttachment(colorAttachment);

    colorText = colorAttach.attachmentId;
    depthText = depthAttach.attachmentId;
    fbo = app->createFramebuffer({ depthAttach,colorAttach },1);
    colorBuffer = GL_COLOR_ATTACHMENT1;
}

LGraphics::LPicking::~LPicking()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &colorText);
	glDeleteTextures(1, &depthText);
}
