#pragma once

#include "../Interfaces/ISystem.h"

/***********************************************************************************/
// Forward Declarations

struct GLFWwindow;

/***********************************************************************************/
class GLWindow : public ISystem {
public:
	GLWindow() = default;
	GLWindow(const GLWindow&) = delete;
	GLWindow& operator=(const GLWindow&) = delete;

	void OnInit(const pugi::xml_node& windowNode) override;
	void OnUpdate(const double delta) override;

	void SetWindowPos(const size_t x, const size_t y) const;
	void SwapBuffers() const;
	void OnSucceed() const override;

	void EnableCursor() const;
	void DisableCursor() const;

	void SetVsync(const bool vsync) const;

	bool ShouldClose() const noexcept { return m_shouldWindowClose; }
	bool IsFocused() const noexcept { return m_focused; }

private:
	void OnAbort(const std::string_view error) override;

	GLFWwindow* m_window = nullptr;

	bool m_shouldWindowClose;
	bool m_focused = true;
};
