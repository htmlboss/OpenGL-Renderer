#pragma once

#include <string_view>

#include "Graphics/GLVertexArray.h"

class Skybox {

public:
	void Init(const std::string_view hdrPath, const std::size_t resolution = 512);
	void Draw();

	auto GetIrradianceMap() const noexcept { return m_irradianceMap; }
	auto GetPrefilterMap() const noexcept { return m_prefilterMap; }
	auto GetBRDFLUT() const noexcept { return m_brdfLUT; }

private:
	void renderCube();

	unsigned int m_cubeVAO, m_envCubemap, m_envMapFBO, m_irradianceMap, m_prefilterMap, m_brdfLUT;

	GLVertexArray m_quadVAO;
};
