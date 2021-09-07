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
		static void drawInstanced();
		void move(const float x, const float y, const float z) override;

	protected:

		static void updateBuffer();
		static void setGlobalUniforms(GLuint shader);
		static void initInstanceBuffer();
		static void resetInstanceBuffer();

		static bool needToResetBuffer;
		static GLuint vbo;
		static std::vector<CubeUniforms> uniforms;

		static std::vector<LCube*> objChanged;
	};
}