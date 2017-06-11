#pragma once

#include "Model.h"

class GLShaderProgram;

class SkySphere {

public:
	SkySphere();

	void Draw(GLShaderProgram* shader, const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat, const glm::vec3& CameraPos);

private:
	Model m_model;
};
