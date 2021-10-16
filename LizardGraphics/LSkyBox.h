#pragma once

#include "LCubeBuffer.h"
#include "LCube.h"
#include <vector>

namespace LGraphics
{
	class LApp;
	class LSkyBoxBuffer : public LCubeBuffer
	{
	public:

		LSkyBoxBuffer(LApp* app);
		void genBuffers() override;
	};

	class LSkyBox : public LCube
	{
	public:

		LSkyBox(LApp* app, ImageSkyboxResource res);
		~LSkyBox();
		void draw() override;

		float skyboxDepth = 0.01f;
	};
}

