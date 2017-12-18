#include "Material.h"

#include "ResourceManager.h"

/***********************************************************************************/
Material::~Material() {
	glDeleteTextures(1, &AlbedoMap);
	glDeleteTextures(1, &AOMap);
	glDeleteTextures(1, &MetallicMap);
	glDeleteTextures(1, &NormalMap);
	glDeleteTextures(1, &RoughnessMap);
}

/***********************************************************************************/
void Material::Init(const std::string_view name, 
					const std::string_view albedoPath, 
					const std::string_view aoPath,
					const std::string_view metallicPath, 
					const std::string_view normalPath,
					const std::string_view roughnessPath) {
	Name = name;

	AlbedoMap = ResourceManager::GetInstance().LoadTexture(albedoPath);
	AOMap = ResourceManager::GetInstance().LoadTexture(aoPath);
	MetallicMap = ResourceManager::GetInstance().LoadTexture(metallicPath);
	NormalMap = ResourceManager::GetInstance().LoadTexture(normalPath);
	RoughnessMap = ResourceManager::GetInstance().LoadTexture(roughnessPath);
}
