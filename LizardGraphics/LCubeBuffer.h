#pragma once
#include "LBuffer.h"

namespace LGraphics
{
	class LCubeBuffer : public LBuffer
	{
	public:

		const char* getObjectType() const override { return "LCubeBuffer"; }

		void setBuffers() override; ///< Инициализирует массив вершин и индексов.
		void setVerts() override; ///< Инициализирует массив вершин.
		void setInds() override; ///< Инициализирует массив индексов.

		void genBuffers() override;

		LCubeBuffer(LApp* app);
	};
}