#include "LApp.h"
#include "LLights.h"

LGraphics::LLight::LLight(LApp* app)
{
	this->app = app;
	//app->addLight(this);
}

LGraphics::LSpotLight::LSpotLight(LApp* app)
	:LLight(app)
{
	app->addLight(this);

	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;

	cutOff = glm::cos(glm::radians(12.5f));

	ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	specular = glm::vec3(1.0f, 1.0f, 1.0f);
}

void LGraphics::LSpotLight::setRadius(float radius)
{
	cutOff = glm::cos(glm::radians(radius));
}

LGraphics::LPointLight::LPointLight(LApp* app)
	:LLight(app)
{
	app->addLight(this);
	constant = 1.0f;
	linear = 0.35f;
	quadratic = 0.44f;

	ambient = glm::vec3(0.2f,0.2f,0.2f);
	diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	specular = glm::vec3(1.0f, 1.0f, 1.0f);
}
