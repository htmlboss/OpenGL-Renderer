#pragma once

#include <glm/vec3.hpp>

struct Light {
	Light(const glm::vec3& position, const glm::vec3& color) noexcept : Position(position), Color(color) {}
	~Light() = default;

	glm::vec3 Position, Color;
};

