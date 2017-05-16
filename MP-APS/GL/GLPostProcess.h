#pragma once
#include "../Interfaces/IRenderComponent.h"

#include "GLShaderProgram.h"
#include "GLFramebuffer.h"

class GLPostProcess : IRenderComponent {
public:
	GLPostProcess(const std::size_t width, const std::size_t height);

	void SetSaturation(const float factor) noexcept { m_saturation = factor; }
	void SetVibrance(const float amount) noexcept { m_vibrance = amount; }

	void Resize(const std::size_t width, const std::size_t height) override;
	void Bind();
	void Update();
	void Shutdown() const;

	void Blit();

private:
	GLShaderProgram m_postProcessShader;

	// HDR
	GLuint m_hdrColorBufferTexture;
	GLFramebuffer m_hdrFBO;

	// Saturation
	float m_saturation = 1.25f;

	// Vibrance
	float m_vibrance = 0.25f;
	glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };
};

