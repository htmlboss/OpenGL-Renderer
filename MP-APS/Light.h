#pragma once

#include <glm/vec3.hpp>

#include <optional>

struct Light {
	// Directional light
	Light(const glm::vec3& color, const glm::vec3& direction) : Color(color),
	                                                            Direction(direction) {
	}

	// Point light
	Light(const glm::vec3& color, const glm::vec3& position, const float constant, const float linear, const float quadratic) : Color(color),
	                                                                                                                            Position(position),
	                                                                                                                            Constant(constant),
	                                                                                                                            Linear(linear),
	                                                                                                                            Quadratic(quadratic) {
	}

	// Spot light
	Light() {
	}

	~Light() = default;

	glm::vec3 Color;

	std::optional<glm::vec3> Position;
	std::optional<glm::vec3> Direction;
	std::optional<float> cutOff;
	std::optional<float> Constant, Linear, Quadratic;
};
