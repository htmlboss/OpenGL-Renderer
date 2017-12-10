#pragma once

#include <glm/vec3.hpp>

struct StaticSpotLight {
	StaticSpotLight(const glm::vec3& color, const glm::vec3& position, const glm::vec3& direction, const float cutoff, const float outerCutoff) : Color(color), 
					Position(position), Direction(direction), Cutoff(cutoff), OuterCutoff(outerCutoff) {}
	
	glm::vec3 Color;
	glm::vec3 Position;
	glm::vec3 Direction;

	float Cutoff;
	float OuterCutoff;
};