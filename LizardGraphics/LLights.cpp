#include "LApp.h"
#include "LLights.h"
#include "LCube.h"

LGraphics::LLight::LLight(LApp* app, size_t shadowWidth, size_t shadowHeight)
{
	this->app = app;
	app->lightsToInit.push_back(this);
	lightSpaceMatrix = glm::mat4();

	ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	specular = glm::vec3(1.0f, 1.0f, 1.0f);

	this->shadowWidth = shadowWidth;
	this->shadowHeight = shadowHeight;
	changed = true;
}

LGraphics::LLight::~LLight()
{
	glDeleteTextures(1, &depthMap);
	glDeleteFramebuffers(1, &depthMapFBO);
}

void LGraphics::LLight::setPosition(glm::vec3 position)
{
	this->position = position;
	changed = true;
}

void LGraphics::LLight::setAmbient(glm::vec3 ambient)
{
	this->ambient = ambient;
	changed = true;
}

void LGraphics::LLight::setDiffuse(glm::vec3 diffuse)
{
	this->diffuse = diffuse;
	changed = true;
}

void LGraphics::LLight::setSpecular(glm::vec3 specular)
{
	this->specular = specular;
	changed = true;
}

void LGraphics::LLight::setNearPlane(float nearPlane)
{
	this->nearPlane = nearPlane;
	changed = true;
}

void LGraphics::LLight::setFarPlane(float farPlane)
{
	this->farPlane = farPlane;
	changed = true;
}

void LGraphics::LLight::setShadowsCalculating(bool calculateShadows)
{
	this->calculateShadow = calculateShadow;
}

void LGraphics::LLight::init()
{
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor_[] = { app->borderColor.x,app->borderColor.y,app->borderColor.z,app->borderColor.w };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor_);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LGraphics::LSpotLight::setLightSpaceMatrix()
{
	LOG_CALL
	glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(position, direction, glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

LGraphics::LSpotLight::LSpotLight(LApp* app)
	:LLight(app)
{
	app->addLight(this);

	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;

	cutOff = glm::cos(glm::radians(12.5f));

	ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	specular = glm::vec3(1.0f, 1.0f, 1.0f);
	//direction = glm::vec3(0.0f, 0.0f, 0.0f);

	nearPlane = 0.1f;
	farPlane = 35.0f;
	calculateShadow = false;
	changed = true;
}

void LGraphics::LSpotLight::setRadius(float radius)
{
	cutOff = glm::cos(glm::radians(radius));
	changed = true;
}

LGraphics::LPointLight::LPointLight(LApp* app)
	:LLight(app)
{
	app->addLight(this);
	constant = 1.0f;
	linear = 0.14f;
	quadratic = 0.07f;

	ambient = glm::vec3(0.2f,0.2f,0.2f);
	diffuse = glm::vec3(0.75f, 0.75f, 0.75f);
	specular = glm::vec3(0.9f, 0.9f, 0.9f);
	calculateShadow = false;
}

void LGraphics::LPointLight::setLightSpaceMatrix()
{
	LOG_CALL
	//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	//glm::mat4 lightView = glm::lookAt(position, direction, glm::vec3(0.0, 1.0, 0.0));
	//lightSpaceMatrix = lightProjection * lightView;
}

void LGraphics::LPointLight::setConstant(float constant)
{
	this->constant = constant;
	changed = true;
}

void LGraphics::LPointLight::setLinear(float linear)
{
	this->linear = linear;
	changed = true;
}

void LGraphics::LPointLight::setQuadratic(float quadratic)
{
	this->quadratic = quadratic;
	changed = true;
}

void LGraphics::LDirectionalLight::setLightSpaceMatrix()
{
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(position, direction, glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

LGraphics::LDirectionalLight::LDirectionalLight(LApp* app)
	:LLight(app)
{
	app->addLight(this);
	position = glm::vec3(0.0f, 3.0f, 5.0f);
	ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	specular = glm::vec3(0.9f, 0.9f, 0.9f);
	direction = glm::vec3(7.5, 0.0f, 7.5f);
	nearPlane = 0.1f;
	farPlane = 35.0f;
	changed = true;
}

void LGraphics::LSpotLight::setCutOff(float cutOff)
{
	this->cutOff = cutOff;
	changed = true;
}

void LGraphics::LSpotLight::setOuterCutOff(float outerCutOff)
{
	this->outerCutOff = outerCutOff;
	changed = true;
}

void LGraphics::LSpotLight::setConstant(float constant)
{
	this->constant = constant;
	changed = true;
}

void LGraphics::LSpotLight::setLinear(float linear)
{
	this->linear = linear;
	changed = true;
}

void LGraphics::LSpotLight::setQuadratic(float quadratic) 
{
	this->quadratic = quadratic;
	changed = true;
}

void LGraphics::LSpotLight::setDirection(glm::vec3 dir)
{
	direction = dir;
	changed = true;
}

void LGraphics::LDirectionalLight::setDirection(glm::vec3 dir)
{
	direction = dir;
	changed = true;
}
