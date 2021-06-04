#pragma once
#include "LShape.h"

namespace LGraphics
{
	class LCircle : public LShape
	{
		friend LApp;

	public:
		
		const char* getObjectType() const override { return "LCircle"; }

		LCircle(LApp* app);

#ifdef VULKAN
		void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) override;
#endif

#ifdef OPENGL
		void draw() override;
#endif
	};
}

