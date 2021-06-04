#pragma once
#include "LWRectangle.h"
#include "LApp.h"

namespace LGraphics
{
	class LCube : public LWRectangle
	{
	public:

		friend LApp;

		LCube(LApp* app, const char* path);
		void draw() override;
	};
}