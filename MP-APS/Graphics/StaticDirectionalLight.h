#pragma once

#include <glm/vec4.hpp>

struct StaticDirectionalLight {
	StaticDirectionalLight() = default;

	StaticDirectionalLight(const glm::vec3& color, const glm::vec3& direction) : Color(color),
																				Direction(direction) {}

	glm::vec3 Color;
	glm::vec3 Direction;
};