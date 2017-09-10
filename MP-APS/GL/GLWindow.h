#pragma once

#include "../Interfaces/ISystem.h"

#include <cstddef>

/***********************************************************************************/
// Forward Declarations

struct GLFWwindow;

/***********************************************************************************/
class GLWindow : public ISystem {
public:
	GLWindow() = default;
	GLWindow(const GLWindow&) = delete;
	GLWindow& operator=(const GLWindow&) = delete;

	void Init(const pugi::xml_node& windowNode) override;
	void Update(const double delta) override;

	void SetWindowPos(const size_t x, const size_t y) const;
	void SwapBuffers() const;
	void DestroyWindow() const;

	void EnableCursor() const;
	void DisableCursor() const;

	void EnableVSync() const;
	void DisableVSync() const;

	bool ShouldClose() const noexcept { return m_shouldWindowClose; }
	bool IsFocused() const noexcept { return m_focused; }

private:

	GLFWwindow* m_window = nullptr;

	bool m_shouldWindowClose;
	bool m_focused = true;
};
