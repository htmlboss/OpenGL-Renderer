#include "PBRMaterial.h"

#include "ResourceManager.h"

/***********************************************************************************/
void PBRMaterial::Init(const std::string_view name, 
					const std::string_view albedoPath, 
					const std::string_view aoPath,
					const std::string_view metallicPath, 
					const std::string_view normalPath,
					const std::string_view roughnessPath,
					const std::string_view alphaMaskPath) 
{
	Name = name;

	m_materialTextures[ALBEDO] = ResourceManager::GetInstance().LoadTexture(albedoPath);
	m_materialTextures[AO] = ResourceManager::GetInstance().LoadTexture(aoPath);
	m_materialTextures[METALLIC] = ResourceManager::GetInstance().LoadTexture(metallicPath);
	m_materialTextures[NORMAL] = ResourceManager::GetInstance().LoadTexture(normalPath);
	m_materialTextures[ROUGHNESS] = ResourceManager::GetInstance().LoadTexture(roughnessPath);
	m_materialTextures[ALPHA] = ResourceManager::GetInstance().LoadTexture(alphaMaskPath);
}

/***********************************************************************************/
unsigned int PBRMaterial::GetParameterTexture(const ParameterType parameter) const noexcept {
	return m_materialTextures[parameter];
}

/***********************************************************************************/
float PBRMaterial::GetParameterColor(const ParameterType parameter) const noexcept {
	return m_materialColors[parameter];
}
