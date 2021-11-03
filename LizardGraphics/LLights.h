#pragma once
#include <climits>

namespace LGraphics
{
	class LApp;
	class LShape;

	class LLight
	{
		friend LApp;
		friend LShape;

	public:
		LLight(LApp* app, size_t shadowWidth = 1024, size_t shadowHeight = 1024);
		virtual ~LLight();

		int getId() const { return id; }
		auto getDepthMap() const{ return depthMap; }
		auto getFBO() const { return depthMapFBO; }

		virtual void setLightSpaceMatrix() {}

		void setPosition(glm::vec3 position);
		void setAmbient(glm::vec3 ambient);
		void setDiffuse(glm::vec3 diffuse);
		void setSpecular(glm::vec3 specular);

		void setNearPlane(float nearPlane);
		void setFarPlane(float farPlane);

		glm::vec3 getPosition() const { return position; }
		glm::vec3 getAmbient() const { return ambient; }
		glm::vec3 getDiffuse() const { return diffuse; }
		glm::vec3 getSpecular() const { return specular; }

		float getNearPlane() const { return nearPlane; }
		float getFarPlane() const { return farPlane; }

		bool getShadowCalculating() const { return calculateShadow; }
		const auto& getLightspaceMatrix() const { return lightSpaceMatrix; }

		void setShadowsCalculating(bool calculateShadows);

	protected:
		void init();

		LApp* app;
		int id;
		GLuint depthMap = UINT32_MAX;
		GLuint depthMapFBO = UINT32_MAX;
		glm::mat4 lightSpaceMatrix;

		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float nearPlane, farPlane;
		bool calculateShadow = true;
		bool changed = true;

		size_t shadowWidth, shadowHeight;
	};

	class LSpotLight : public LLight
	{
	public:

		friend LShape;

		LSpotLight(LApp* app);
		~LSpotLight(){}

		void setRadius(float radiusDegrees);

		void setCutOff(float cutOff);
		void setOuterCutOff(float outerCutOff);
		void setConstant(float constant);
		void setLinear(float linear);
		void setQuadratic(float quadratic);
		void setDirection(glm::vec3 dir);

		float getCutOff() const { return cutOff; }
		float getOuterCutOff()  const { return outerCutOff; }
		float getConstant() const { return constant; }
		float getLinear() const { return linear; }
		float getQuadratic() const { return quadratic; }
		glm::vec3 getDirection() const { return direction; }

		void setLightSpaceMatrix() override;

	protected:
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;
		float constant;
		float linear;
		float quadratic;
	};

	class LPointLight : public LLight
	{
	public:

		friend LShape;

		LPointLight(LApp* app);
		~LPointLight(){}

		void setLightSpaceMatrix() override;
		void setConstant(float constant);
		void setLinear(float linear);
		void setQuadratic(float quadratic);

		float getConstant() const { return constant; }
		float getLinear() const { return linear; }
		float getQuadratic() const { return quadratic; }

	protected:
		float constant;
		float linear;
		float quadratic;
	};

	class LDirectionalLight : public LLight
	{
	public:
		LDirectionalLight(LApp* app);
		void setLightSpaceMatrix() override;
		void setDirection(glm::vec3);

		glm::vec3 getDirection() const { return direction; }

	protected:
		glm::vec3 direction;
	};
}