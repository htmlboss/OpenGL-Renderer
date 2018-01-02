#include "PBRMaterial.h"

#include "ResourceManager.h"

/***********************************************************************************/
void PBRMaterial::Init(const std::string_view name, 
					const std::string_view albedoPath, 
					const std::string_view aoPath,
					const std::string_view metallicPath, 
					const std::string_view normalPath,
					const std::string_view roughnessPath,
					const std::string_view alphaMaskPath) {
	Name = name;

	AlbedoMap = ResourceManager::GetInstance().LoadTexture(albedoPath);
	AOMap = ResourceManager::GetInstance().LoadTexture(aoPath);
	MetallicMap = ResourceManager::GetInstance().LoadTexture(metallicPath);
	NormalMap = ResourceManager::GetInstance().LoadTexture(normalPath);
	RoughnessMap = ResourceManager::GetInstance().LoadTexture(roughnessPath);
	AlphaMask = ResourceManager::GetInstance().LoadTexture(alphaMaskPath);
}
