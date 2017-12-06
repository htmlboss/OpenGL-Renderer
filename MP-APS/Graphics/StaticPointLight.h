#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

struct StaticPointLight {
	StaticPointLight() {}
	
	StaticPointLight(const glm::vec3& color, const glm::vec3& position, const float radius) : Color(color, 1.0f),
																						Position(position, 1.0f),
																						RadiusAndPadding(radius, glm::vec3(0.0f)) { }

	glm::vec4 Color;
	glm::vec4 Position;
	glm::vec4 RadiusAndPadding;
};