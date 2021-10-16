#pragma once
#include "LBuffer.h"

namespace LGraphics
{
	class LCubeBuffer : public LBuffer
	{
	public:

		const char* getObjectType() const override { return "LCubeBuffer"; }
		void genBuffers() override;

		LCubeBuffer(LApp* app);

	private:
		static const std::vector<Vertex> vert;
	};
}