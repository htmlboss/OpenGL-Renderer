#pragma once

#include <string_view>

struct GLFWwindow;

class GLWindow {
public:
	GLWindow() = default;

	GLWindow(const GLWindow&) = delete;
	GLWindow& operator=(const GLWindow&) = delete;

	void Init(const size_t width, const size_t height, const std::string_view windowName, const bool fullscreen = false);

	void PollEvents() const;
	void SetWindowPos(const size_t x, const size_t y) const;
	void SwapBuffers() const;
	void DestroyWindow() const;

	void EnableCursor() const;
	void DisableCursor() const;

	void EnableVSync() const;
	void DisableVSync() const;

	void Update();
	bool ShouldClose() const noexcept { return m_shouldWindowClose; }
	bool IsFocused() const noexcept { return m_focused; }

private:
	GLFWwindow* m_window = nullptr;

	bool m_shouldWindowClose;
	bool m_focused = true;
};
