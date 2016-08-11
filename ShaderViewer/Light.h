#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader.h"

class Light {
public:
	Light(const glm::vec3& Position, const glm::vec3& Color);
	~Light();

	void Draw(Shader& shader, const glm::mat4& cameraMat, const glm::mat4& projMat);

private:
	glm::vec3 m_position;
	glm::vec3 m_color;

	GLuint m_vao;
	GLuint m_vbo;
};