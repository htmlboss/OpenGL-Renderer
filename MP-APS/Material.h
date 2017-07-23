#pragma once

#include <glm/vec3.hpp>

// Material for a PBR pipeline
struct Material {
	explicit Material() = default;
	Material(const glm::vec3& albedo, const float metallic, const float ao, const float roughness) :	Albedo(albedo),
																										Metallic(metallic),
																										AO(ao),
																										Roughness(roughness) {}

	glm::vec3 Albedo = {0.5f, 0.5f, 0.0f};
	float Metallic = 1.0f;
	float AO = 0.0f;
	float Roughness = 0.5f;
};
