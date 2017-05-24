#pragma once

#include <cstddef>

class IWindow {
public:
	constexpr IWindow(const std::size_t width, const std::size_t height) noexcept : m_shouldWindowClose(false), m_width(width), m_height(height) {}
	virtual ~IWindow() = default;

	IWindow(const IWindow&) = delete;
	IWindow& operator=(const IWindow&) = delete;

protected:
	virtual void PollEvents() const = 0;
	virtual void SetWindowPos(const std::size_t x, const std::size_t y) const = 0;
	virtual void SwapBuffers() const = 0;
	virtual void DestroyWindow() const = 0;

	mutable bool m_shouldWindowClose;

	std::size_t m_width, m_height;
};