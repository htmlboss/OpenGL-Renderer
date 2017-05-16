#pragma once

#include "../Interfaces/IWindow.h"

#include <string_view>
#include <functional>

struct GLFWwindow;

class GLWindow : IWindow {
public:
	GLWindow(const std::size_t width, const std::size_t height, const std::string_view windowName, const bool fullscreen = false);
	~GLWindow() = default;

	GLWindow(const GLWindow&) = delete;
	GLWindow& operator=(const GLWindow&) = delete;
	
	void PollEvents() const override;
	void SetWindowPos(const std::size_t x, const std::size_t y) const override;
	void SwapBuffers() const override;
	void DestroyWindow() const override;

	void EnableCursor() const;
	void DisableCursor() const;

	static void EnableVSync();
	static void DisableVSync();

	void Update() const;
	bool ShouldClose() const noexcept { return m_shouldWindowClose; }
	bool IsFocused() const noexcept { return m_focused; }
	
private:
	GLFWwindow* m_window;

	bool m_focused = true;
};

