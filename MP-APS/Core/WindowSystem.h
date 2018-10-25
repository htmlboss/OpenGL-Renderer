#pragma once

#include <cstddef>
#include <utility>

/***********************************************************************************/
// Forward Declarations
namespace pugi {
class xml_node;
}
class Engine;
struct GLFWwindow;

/***********************************************************************************/
class WindowSystem {
	friend class Engine;
public:
	WindowSystem() noexcept = default;

	WindowSystem(WindowSystem&&) = default;
	WindowSystem(const WindowSystem&) = delete;
	WindowSystem& operator=(WindowSystem&&) = default;
	WindowSystem& operator=(const WindowSystem&) = delete;

	~WindowSystem() = default;

	void Init(const pugi::xml_node& windowNode);
	void Update();
	void Shutdown() const;

	void SetWindowPos(const std::size_t x, const std::size_t y) const;
	void SwapBuffers() const;

	void EnableCursor() const;
	void DisableCursor() const;

	void SetVsync(const bool vsync) const;

	auto ShouldClose() const noexcept { return m_shouldWindowClose; }
	auto IsCursorVisible() const noexcept { return m_showCursor; }

	// Returns the window's framebuffer dimensions in pixels {width, height}.
	std::pair<int, int> GetFramebufferDims() const;

private:
	GLFWwindow* m_window{ nullptr };

	bool m_shouldWindowClose{ false };
	bool m_showCursor{ false };
};
