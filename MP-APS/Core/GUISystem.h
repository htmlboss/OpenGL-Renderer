#pragma once

/***********************************************************************************/
// Forward Declarations
struct nk_context;
struct GLFWwindow;

/***********************************************************************************/
class GUISystem {
public:
	GUISystem() noexcept = default;

	GUISystem(GUISystem&&) = delete;
	GUISystem(const GUISystem&) = delete;
	GUISystem& operator=(GUISystem&&) = delete;
	GUISystem& operator=(const GUISystem&) = delete;

	void Init(GLFWwindow* windowPtr);
	void Update();
	void Render();
	void Shutdown() const;

private:
	nk_context* m_nuklearContext{ nullptr };
};
