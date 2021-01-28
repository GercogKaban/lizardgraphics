#define FROM_DLL 1
#include "optimized.h"
#include "../LizardGraphics/include/glm/glm.hpp"
#include "../LizardGraphics/include/glm/gtc/matrix_transform.hpp"

glm::mat4 _calculateModelMatrix(const glm::vec3& move_, 
	const glm::mat4& rotate_, const glm::vec3& scale_)
{
	glm::mat4 model_ = glm::mat4(1.0f);
	model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
	model_ *= rotate_;
	model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
	return model_;
}