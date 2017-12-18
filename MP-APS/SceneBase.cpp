#include "SceneBase.h"

#include "Input.h"
#include "ViewFrustum.h"
#include "ResourceManager.h"

#include <ppl.h> // Concurrency library

/***********************************************************************************/
SceneBase::SceneBase(const std::size_t width, const std::size_t height) :	m_width(width), 
																			m_height(height) {
}

/***********************************************************************************/
void SceneBase::Init() {

	auto model = ResourceManager::GetInstance().GetModel("Sponza", "Data/Models/crytek-sponza/sponza.obj");
	model->Translate(glm::vec3(0.0f));
	model->Scale(glm::vec3(0.1f));
	m_sceneModels.push_back(model);

	//model = ResourceManager::GetInstance().GetModel("Dragon", "models/dragon/dragon.obj");
	//model->Translate(glm::vec3(20.0f, -50.0f, 0.0f));
	//model->Scale(glm::vec3(2.0f));
	//m_sceneModels.push_back(model);

	// Sun
	m_staticDirectionalLights.emplace_back(StaticDirectionalLight(glm::vec3(300.0f), { -0.2f, -1.0f, -0.3f}));
	// other suns?
	//m_staticDirectionalLights.emplace_back(StaticDirectionalLight({ 0.0f, 0.25f, 1.0f }, { -0.5f, -1.5f, -0.5f }));
	//m_staticDirectionalLights.emplace_back(StaticDirectionalLight({ 1.0f, 0.0f, 0.0f }, { -0.75f, -0.5f, -0.5f }));

	// Random lights
	//m_staticPointLights.emplace_back(StaticPointLight(glm::vec3(300.0f), glm::vec3(0.0f)));
	//m_staticPointLights.emplace_back(StaticPointLight(2.0f, 0.0f}, {5.0f, 2.0f, 0.0f}));
	//m_staticPointLights.emplace_back(StaticPointLight({ 0.0f, 2.0f, 0.0f }, { 5.0f, 5.0f, 5.0f }));
	//m_staticPointLights.emplace_back(StaticPointLight({ 0.0f, 0.0f, 5.0f }, { 10.0f, 2.0f, 0.0f }));
}

/***********************************************************************************/
void SceneBase::Update(const double delta) {

	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();
	}
	m_camera.Update(delta);

	cullViewFrustum();
}

/***********************************************************************************/
void SceneBase::cullViewFrustum() {
	// TODO: optimize projection matrix calculation
	ViewFrustum viewFrustum(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_width, m_height));
	m_renderList.clear();

	concurrency::parallel_for_each(m_sceneModels.cbegin(), m_sceneModels.cend(), [&](const auto model) {
		const auto result = viewFrustum.TestIntersection(model->GetBoundingBox());

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			m_renderList.push_back(model);
		}
	});
}