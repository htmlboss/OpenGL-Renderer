#pragma once

#include <glm/vec3.hpp>

#include <memory>
#include <array>
#include <variant>

// Material for a PBR pipeline
class PBRMaterial {
public:
	PBRMaterial();

	enum ParameterType {
		ALBEDO = 0,
		AO,
		METALLIC,
		NORMAL,
		ROUGHNESS
	};

	void Init(const std::string_view name,
		const std::string_view albedoPath,
		const std::string_view aoPath,
		const std::string_view metallicPath,
		const std::string_view normalPath,
		const std::string_view roughnessPath,
		const std::string_view alphaMaskPath);

	void Init(const std::string_view name,
		const glm::vec3& albedo,
		const glm::vec3& ao,
		const glm::vec3& metallic,
		const glm::vec3& normal,
		const glm::vec3& roughness,
		const float alpha = 1.0f);

	auto operator==(const PBRMaterial& rhs) const noexcept {
		return Name == rhs.Name;
	}

	std::string_view Name;

	unsigned int GetParameterTexture(const ParameterType parameter) const noexcept;
	glm::vec3 GetParameterColor(const ParameterType parameter) const noexcept;

	// Gets active alpha texture/value from union
	auto GetAlpha() const noexcept;

private:
	std::variant<unsigned int, float> m_alpha;

	// unsigned int HeightMap;

	std::array<unsigned int, 5> m_materialTextures;
	std::array<glm::vec3, 5> m_materialColors;
};

using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;