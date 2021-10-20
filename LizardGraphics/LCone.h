#pragma once
#include "LImagedShape.h"

namespace LGraphics
{
	class LApp;
	class LCone : public LImagedShape
	{
	public:

		friend LApp;

		LCone(LApp* app, ImageResource res);
		void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override {}
		static void drawInstanced();

		void setModel(const glm::mat4& model) override;
		void scale(const float x, const float y, const float z) override;
		void move(const float x, const float y, const float z) override;

		void rotateX(float angle) override;
		void rotateY(float angle) override;
		void rotateZ(float angle) override;

		void setNormalMapping(bool normalMapping) override;
		void setParallaxMapping(bool parallaxMapping) override;

	protected:

		static void updateBuffer();
		static void initInstanceBuffer();
		static void resetInstanceBuffer();

		static bool needToResetBuffer;
		static GLuint vbo;
		static std::vector<LWidget::PrimitiveUniforms> uniforms;

		static std::vector<LShape*> objChanged;
	};
}