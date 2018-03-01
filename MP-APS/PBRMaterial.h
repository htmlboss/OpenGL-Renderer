#pragma once

#include <memory>
#include <array>

// Material for a PBR pipeline
class PBRMaterial {

public:

	enum ParameterType {
		ALBEDO = 0,
		AO,
		METALLIC,
		NORMAL,
		ROUGHNESS,
		ALPHA
	};

	void Init(const std::string_view name,
		const std::string_view albedoPath,
		const std::string_view aoPath,
		const std::string_view metallicPath,
		const std::string_view normalPath,
		const std::string_view roughnessPath,
		const std::string_view alphaMaskPath);

	auto operator==(const PBRMaterial& rhs) const noexcept {
		return Name == rhs.Name;
	}

	std::string_view Name;

	unsigned int GetParameterTexture(const ParameterType parameter) const noexcept;
	float GetParameterColor(const ParameterType parameter) const noexcept;

private:
	// unsigned int HeightMap;

	std::array<unsigned int, 6> m_materialTextures;
	std::array<float, 6> m_materialColors;
};

using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;