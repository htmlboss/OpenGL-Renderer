#pragma once

#include <glm/vec4.hpp>

struct DirectionalLight {
	DirectionalLight() noexcept {}

	DirectionalLight(const glm::vec3& direction, const glm::vec3& color) :	Direction(direction, 1.0f),
																			Color(color, 1.0f) {}

	glm::vec4 Direction;
	glm::vec4 Color;
};