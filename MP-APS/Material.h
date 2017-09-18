#pragma once

#include <glm/vec3.hpp>

#include <memory>

// Material for a PBR pipeline
struct Material {
	explicit Material() = default;
	Material(const std::string_view name, const glm::vec3& albedo, const float metallic, const float ao, const float roughness) :	Name(name),
																																	Albedo(albedo),
																																	Metallic(metallic),
																																	AO(ao),
																																	Roughness(roughness) {}

	// Specialize std::swap
	friend void swap(Material& first, Material& second) noexcept {
		using std::swap;

		swap(first.Name, second.Name);
		swap(first.Albedo, second.Albedo);
		swap(first.Metallic, second.Metallic);
		swap(first.AO, second.AO);
		swap(first.Roughness, second.Roughness);
	}
	
	// Good ole copy-n-swap
	// https://stackoverflow.com/a/3279550/2231969
	Material& operator=(Material rhs) noexcept {
		std::swap(*this, rhs);
		return *this;
	}

	auto operator==(const Material& rhs) const noexcept {
		return Name == rhs.Name;
	}

	std::string Name;
	glm::vec3 Albedo = {2.0f, 30.0f, 10.0f};
	float Metallic = 1.0f;
	float AO = 1.0f;
	float Roughness = 0.01f;
};
using MaterialPtr = std::shared_ptr<Material>;