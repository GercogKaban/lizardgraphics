#include "LPicking.h"
#include "LApp.h"

LGraphics::LPicking::LPicking(LApp* app, size_t width, size_t height)
{
	this->width = width;
    this->height = height;

    LApp::FBOAttach depthAttachment, colorAttachment;
    depthAttachment.attachmentSize = { width,height };
    depthAttachment.componentType = GL_DEPTH_COMPONENT;
    depthAttachment.textureType = GL_TEXTURE_2D;
    depthAttachment.valuesType = GL_FLOAT;
    depthAttachment.drawBuffer = GL_DEPTH_ATTACHMENT;

    colorAttachment = depthAttachment;
    colorAttachment.componentType = GL_RGB32UI;
    colorAttachment.valuesType = GL_UNSIGNED_INT;
    colorAttachment.drawBuffer = GL_COLOR_ATTACHMENT1;

    const auto depthAttach = app->createAttachment(depthAttachment);
    const auto colorAttach = app->createAttachment(colorAttachment);

    colorText = colorAttach.attachmentId;
    depthText = depthAttach.attachmentId;
    fbo = app->createFramebuffer({ depthAttach,colorAttach });
    colorBuffer = colorAttachment.drawBuffer;
}

LGraphics::LPicking::~LPicking()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &colorText);
	glDeleteTextures(1, &depthText);
}
