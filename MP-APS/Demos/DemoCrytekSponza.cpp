#include "DemoCrytekSponza.h"

#include "../ResourceManager.h"


/***********************************************************************************/
DemoCrytekSponza::DemoCrytekSponza(const std::size_t width, const std::size_t height) : SceneBase(width, height) {
}

/***********************************************************************************/
void DemoCrytekSponza::Init(const std::string_view sceneName) {
	SceneBase::Init(sceneName);

	auto model = ResourceManager::GetInstance().GetModel("Sponza", "Data/Models/crytek-sponza/sponza.obj");
	model->Translate(glm::vec3(0.0f));
	model->Scale(glm::vec3(0.01f));
	AddModel(model);

	// Sun
	AddLight(StaticDirectionalLight({ 5.0f, 5.0f, 4.5f }, { 25.0f, 50.0f, 10.0f }));
}

