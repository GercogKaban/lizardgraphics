#pragma once

namespace LGraphics
{
	class LApp;
	class LLight
	{
		friend LApp;
	public:
		LLight(LApp* app);
		~LLight(){}

		virtual int getId() const { return id; }

	protected:
		LApp* app;
		int id;
	};

	class LSpotLight : public LLight
	{
	public:
		LSpotLight(LApp* app);
		~LSpotLight(){}

		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float cutOff;
		float outerCutOff;
		float constant;
		float linear;
		float quadratic;

		bool calculateShadow = true;

		void setRadius(float radius);
	};

	class LPointLight : public LLight
	{
	public:
		LPointLight(LApp* app);
		~LPointLight(){}

		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float constant;
		float linear;
		float quadratic;

		bool calculateShadow = false;
	};
}