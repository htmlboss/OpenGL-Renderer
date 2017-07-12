#pragma once

#include <glm/vec4.hpp>

struct PointLight {
	PointLight() noexcept {}
	
	PointLight(const glm::vec4& color, const glm::vec3& position, const float radius) : Color(color),
																						Position(position, 1.0f),
																						RadiusAndPadding(radius, glm::vec3(0.0f)) { }

	glm::vec4 Color;
	glm::vec4 Position;
	glm::vec4 RadiusAndPadding;
};