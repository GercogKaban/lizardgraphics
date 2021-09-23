#pragma once

#include "LBuffer.h"
#include "LCube.h"
#include <vector>

namespace LGraphics
{
	class LApp;
	class LSkyBoxBuffer : public LBuffer
	{
	public:

		LSkyBoxBuffer(LApp* app);

		void setBuffers() override;
		void setVerts() override;
		void setInds() override {}
		void genBuffers() override;
	};

	class LSkyBox : public LCube
	{
	public:

		LSkyBox(LApp* app, const std::vector<std::string>& paths);
		void draw() override;

		float skyboxDepth = 0.01f;
	};
}

