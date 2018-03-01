#include "PBRMaterial.h"

#include "ResourceManager.h"

/***********************************************************************************/
PBRMaterial::PBRMaterial() {
	// Set material defaults
	std::fill(m_materialTextures.begin(), m_materialTextures.end(), 0);
	std::fill(m_materialColors.begin(), m_materialColors.end(), glm::vec3(0.0f));
}

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
	
	m_alpha = ResourceManager::GetInstance().LoadTexture(alphaMaskPath);
}

/***********************************************************************************/
void PBRMaterial::Init(const std::string_view name,
					const glm::vec3& albedo,
					const glm::vec3& ao, 
					const glm::vec3& metallic,
					const glm::vec3& normal, 
					const glm::vec3& roughness, 
					const float alpha)
{
	Name = name;

	m_materialColors[ALBEDO] = albedo;
	m_materialColors[AO] = ao;
	m_materialColors[METALLIC] = metallic;
	m_materialColors[NORMAL] = normal;
	m_materialColors[ROUGHNESS] = roughness;

	m_alpha = alpha;
}

/***********************************************************************************/
unsigned int PBRMaterial::GetParameterTexture(const ParameterType parameter) const noexcept {
	return m_materialTextures[parameter];
}

/***********************************************************************************/
glm::vec3 PBRMaterial::GetParameterColor(const ParameterType parameter) const noexcept {
	return m_materialColors[parameter];
}
