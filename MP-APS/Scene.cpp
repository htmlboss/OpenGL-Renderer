#include "Scene.h"
#include "Input.h"
#include "ViewFrustum.h"

#include "ResourceManager.h"
#include "Utils/Utils.h"

#include <ppl.h>

/***********************************************************************************/
Scene::Scene(const size_t width, const size_t height) :	m_renderData(0, 0),
														m_width(width), 
														m_height(height) {
}

/***********************************************************************************/
void Scene::Init() {

	auto model = ResourceManager::GetInstance().GetModel("Sphere", "models/sphere.obj");
	model->Translate({ 0.0f, 0.0f, 0.0f });
	model->Scale(glm::vec3(10.0f));
	m_sceneModels.push_back(model);

	// Sun parameters
	m_renderData.Sun.Color = { 0.0f, 0.5f, 1.0f , 1.0f};
	m_renderData.Sun.Direction = { -0.2f, -1.0f, -0.3f , 0.0f};
}

/***********************************************************************************/
void Scene::Update(const double delta) {

	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();
	}
	m_camera.Update(delta);

	cullViewFrustum();
}

/***********************************************************************************/
void Scene::cullViewFrustum() {
	ViewFrustum viewFrustum(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_width, m_height));
	m_renderData.renderList.clear();

	concurrency::parallel_for_each(m_sceneModels.cbegin(), m_sceneModels.cend(), [&](const auto model) {
		const auto result = viewFrustum.TestIntersection(model->GetBoundingBox());

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			m_renderData.renderList.push_back(model);
		}
	});
}