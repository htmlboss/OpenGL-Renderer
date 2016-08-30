#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

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
	// Only used for inheritance
	struct BasicLight {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;

	};

	// Add direction
	struct DirLight : BasicLight {
		glm::vec3 direction;
	};

	// Add attenuation parameters
	struct PointLight : BasicLight {
		glm::vec3 position;
		
		float constant;
		float linear;
		float quadratic;
	};
	
	// Add cutoffs (and avoid The Diamond)
	struct SpotLight : BasicLight {
		glm::vec3 direction;
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		// For smooth edges
		float cutOff;
		float outerCutOff;
	};

	glm::vec3 m_position;
	glm::vec3 m_color;

	GLuint m_vao;
	GLuint m_vbo;
};