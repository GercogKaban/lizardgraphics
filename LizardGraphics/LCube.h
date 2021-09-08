#pragma once
#include "LWRectangle.h"
#include "LApp.h"
#include <set>

#define PARALLEL_UPDATE

namespace LGraphics
{
	class LCube : public LWRectangle
	{
	public:

		friend LApp;

		LCube(LApp* app, const char* path);
		void draw() override;
		static void drawInstanced();

		void setModel(const glm::mat4& model) override;
		void scale(const float x, const float y, const float z) override;
		void move(const float x, const float y, const float z) override;

		void rotateX(float angle) override;
		void rotateY(float angle) override;
		void rotateZ(float angle) override;

	protected:

		static void updateBuffer();
		static void setGlobalUniforms(GLuint shader);
		static void initInstanceBuffer();
		static void resetInstanceBuffer();

		static void updateBufferParallel(LWidget::CubeUniforms* buffer, std::vector<LCube*>& changed, size_t begin, size_t end);

		static bool needToResetBuffer;
		static GLuint vbo;
		static std::vector<CubeUniforms> uniforms;


#ifdef EXPERIMENTAL_SET
		static std::set<LCube*> objChanged;
#else
		static std::vector<LCube*> objChanged;
#endif
	};
}