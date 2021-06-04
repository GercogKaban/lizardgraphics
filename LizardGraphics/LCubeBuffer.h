#pragma once
#include "LBuffer.h"

namespace LGraphics
{
	class LCubeBuffer : public LBuffer
	{
	public:

		const char* getObjectType() const override { return "LCubeBuffer"; }

		void setBuffers() override; ///< �������������� ������ ������ � ��������.
		void setVerts() override; ///< �������������� ������ ������.
		void setInds() override; ///< �������������� ������ ��������.

		void genBuffers() override;

		LCubeBuffer(LApp* app);
	};
}