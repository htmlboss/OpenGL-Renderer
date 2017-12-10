#pragma once

#include <string_view>

class Skybox {
public:
	Skybox() = default;

	void Init(const std::string_view hdrPath, const std::size_t resolution = 512);
	void Draw();

	auto GetIrradianceMap() const noexcept { return m_irradianceMap; }

private:
	void renderCube();

	unsigned int m_vao, m_envMap, m_envMapFBO, m_irradianceMap;
};
