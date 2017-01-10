#include "SkySphere.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>

/***********************************************************************************/
SkySphere::SkySphere() : m_model("models/sphere.obj", "SkySphere", true) {
}

/***********************************************************************************/
SkySphere::~SkySphere() {
}

/***********************************************************************************/
void SkySphere::Draw(Shader& shader, const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat, const glm::vec3& CameraPos) {
	glDepthFunc(GL_LEQUAL);
	shader.Bind();
	glActiveTexture(GL_TEXTURE0);

	const auto view = glm::mat4(glm::mat3(CameraMatrix)); // Remove any translation component of the view matrix
	shader.SetUniform("view", view);
	shader.SetUniform("projection", ProjectionMat);

	m_model.Draw(shader);

	glDepthFunc(GL_LESS);
}
