#pragma once

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Light {
public:
	
	enum LightType {
		POINTLIGHT,
		SPOTLIGHT,
		DIRLIGHT
	};

	Light(const glm::vec3& Position, const glm::vec3& Color, const LightType lightType);
	~Light();

	void Draw(Shader& shader);

private:
	glm::vec3 m_position;
	glm::vec3 m_color;

	GLuint m_vao, m_vbo;
};