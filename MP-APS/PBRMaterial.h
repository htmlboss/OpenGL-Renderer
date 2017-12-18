#pragma once

#include <string_view>

// Material for a PBR pipeline
struct PBRMaterial {

	void Init(const std::string_view name,
		const std::string_view albedoPath,
		const std::string_view aoPath,
		const std::string_view metallicPath,
		const std::string_view normalPath,
		const std::string_view roughnessPath);

	auto operator==(const PBRMaterial& rhs) const noexcept {
		return Name == rhs.Name;
	}

	std::string_view Name;

	unsigned int AlbedoMap;
	unsigned int AOMap;
	unsigned int MetallicMap;
	unsigned int NormalMap;
	unsigned int RoughnessMap;
	// unsigned int HeightMap;
};