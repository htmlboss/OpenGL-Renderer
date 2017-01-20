#pragma once

class IRenderer {

public:
	IRenderer(const size_t width, const size_t height);
	virtual ~IRenderer();

	virtual void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 0.0f) const = 0;
	virtual void Resize(const size_t width, const size_t height) = 0;

protected:
	size_t m_width, m_height;
};

