#pragma once
#include "../LizardGraphics/include/glm/glm.hpp"
#include "../LizardGraphics/vectors.h"

#if FROM_DLL
#define F_HEADER //__declspec(dllexport)
#else
#define F_HEADER __declspec(dllimport)
#endif

F_HEADER glm::mat4 _calculateModelMatrix(const fvect3& move_, const glm::mat4& rotate_, const fvect3& scale_);