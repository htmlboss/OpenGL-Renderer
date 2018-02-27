#include "DemoPathfinding.h"

#include "../ResourceManager.h"


/***********************************************************************************/
DemoPathfinding::DemoPathfinding(const std::size_t width, const std::size_t height) : SceneBase(width, height) {
}

/***********************************************************************************/
void DemoPathfinding::Init(const std::string_view sceneName) {
	SceneBase::Init(sceneName);

	m_camera.SetNear(0.1f);
	m_camera.SetSpeed(2.0);

	auto model = ResourceManager::GetInstance().GetModel("Plane", "Data/Models/plane.obj");
	model->Translate(glm::vec3(0.0f));
	model->Scale(glm::vec3(4.0f, 2.0f, 4.0f));
	AddModel(model);

	// Sun
	AddLight(StaticDirectionalLight({ 5.0f, 5.0f, 4.5f }, { 25.0f, 50.0f, 10.0f }));
}
