#pragma once

#include "Model.h"

class Shader;

class SkySphere {

public:
	SkySphere();
	~SkySphere();

	void Draw(Shader& shader, const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat, const glm::vec3& CameraPos);

private:
	Model m_model;
};

