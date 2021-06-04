#pragma once

#include "LCube.h"

namespace LGraphics
{
	class LRectangleMirror : public LCube
	{
	public:
		LRectangleMirror(LApp* app);
		void draw() override;
	};
}