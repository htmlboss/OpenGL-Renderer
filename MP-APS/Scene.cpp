#include "Scene.h"
#include "Input.h"
#include "ViewFrustum.h"

#include "ResourceManager.h"
#include "Utils/Utils.h"


/***********************************************************************************/
Scene::Scene(const size_t width, const size_t height) noexcept : m_width(width), m_height(height), m_terrain(0, 0, 10) {
}

/***********************************************************************************/
void Scene::Init() {

	auto model = ResourceManager::GetInstance().GetModel("Dragon", "models/dragon/dragon.obj");
	model->Translate({10.0f, 0.0f, 10.0f});
	model->Scale({3.0f, 3.0f, 3.0f});
	m_sceneModels.push_back(model);
}

/***********************************************************************************/
void Scene::Update(const double delta) {

	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();
	}
	m_camera.Update(delta);

	// Frustum culling (and repopulate render list to be sent to renderer)
	ViewFrustum viewFrustum(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_width, m_height));
	m_renderList.clear();

	for (const auto& model : m_sceneModels) {
		const auto result = viewFrustum.TestIntersection(model->GetBoundingBox());

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			m_renderList.push_back(model);
		}
	}
}
