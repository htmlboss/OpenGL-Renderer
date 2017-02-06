#include "IRenderer.h"

std::array<bool, 1024> IRenderer::m_keys;

/***********************************************************************************/
IRenderer::IRenderer(const size_t width, const size_t height) : m_width(width), m_height(height), m_timer() {
}

/***********************************************************************************/
IRenderer::~IRenderer() {
}
