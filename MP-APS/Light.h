#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light {

	Light() = default;

	// Directional light
	Light(const glm::vec3& color, const glm::vec3& direction) : Color(color),
	                                                            Direction(direction) {
		const auto lightView = glm::lookAt(direction, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
		const auto projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 1000.0f);
		
		m_lightSpaceMatrix = projection * lightView;
	}

	// Point light
	Light(const glm::vec3& color, const glm::vec3& position, const float constant, const float linear, const float quadratic) : Color(color),
	                                                                                                                            Position(position),
	                                                                                                                            Constant(constant),
	                                                                                                                            Linear(linear),
	                                                                                                                            Quadratic(quadratic) {
	}

	// Spot light}

	auto GetLightSpaceMatrix() const noexcept { return m_lightSpaceMatrix; }

	glm::vec3 Color;

	glm::vec3 Position;
	glm::vec3 Direction;
	float cutOff;
	float Constant, Linear, Quadratic;

private:
	glm::mat4x4 m_lightSpaceMatrix;
};