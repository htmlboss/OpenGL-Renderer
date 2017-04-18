#pragma once
#include "glad/glad.h"

#include "GLShaderProgram.h"

class GBuffer {
public:
	GBuffer(const std::size_t width, const std::size_t height);
	~GBuffer();

	void Shutdown();

	void BindGBuffer() const;
	void UnBindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void BindGeometryShader() const;
	void BindLightingShader() const;

	void Resize(const std::size_t width, std::size_t height);
	
	void SetCameraPos(const glm::vec3& pos);

	void BindTextures() const;

	// Write depth buffer to default framebuffer
	void BlitDepthBuffer() const;

	GLShaderProgram* GetGeometryShader() { return &m_geometryPassShader; }
private:
	GLuint m_gBuffer, m_gPosition, m_gNormal, m_gDiffuseSpec, m_rboDepth;
	std::size_t m_width, m_height;

	GLShaderProgram m_geometryPassShader, m_lightingPassShader;
};

