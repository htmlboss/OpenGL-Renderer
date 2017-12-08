#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

struct StaticPointLight {
	StaticPointLight(const glm::vec3& color, const glm::vec3& position) : Color(color),
																						Position(position) { }

	glm::vec3 Color;
	glm::vec3 Position;
};