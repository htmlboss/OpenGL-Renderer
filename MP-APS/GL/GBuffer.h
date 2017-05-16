#pragma once
#include "../Interfaces/IRenderComponent.h"

#include "GLShaderProgram.h"
#include "GLFramebuffer.h"

class GBuffer : public IRenderComponent {
public:
	GBuffer(const std::size_t width, const std::size_t height);
	~GBuffer() = default;

	void Shutdown();

	void BindGBuffer() const;
	void UnBindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void BindGeometryShader() const;
	void BindLightingShader() const;

	void Resize(const std::size_t width, const std::size_t height) override;
	
	void SetCameraPos(const glm::vec3& pos);

	void BindPosNorm() const;
	void BindPosNormDiffSpec() const;

	// Write depth buffer to default framebuffer
	void BlitDepthBuffer() const;

	GLShaderProgram* GetGeometryShader() { return &m_geometryPassShader; }
private:
	GLFramebuffer m_gBufferFB;
	GLuint m_gPosition, m_gNormal, m_gDiffuseSpec, m_rboDepth;

	GLShaderProgram m_geometryPassShader, m_lightingPassShader;
};

