#pragma once
#include "IRenderer.h"
#include "GBuffer.h"

#include <memory>

class Renderer : public IRenderer {
public:
	Renderer(const size_t width, const size_t height);
	~Renderer();

	void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 0.0f) const;
	void Resize(const size_t width, const size_t height);

	std::unique_ptr<GBuffer> m_gBuffer;
};
