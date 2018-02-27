#include "SceneBase.h"

#include "Input.h"
#include "ViewFrustum.h"

#include <ppl.h> // Concurrency library
#include <iostream>

/***********************************************************************************/
SceneBase::SceneBase(const std::size_t width, const std::size_t height) :	m_width(width), 
																			m_height(height) {
}

/***********************************************************************************/
void SceneBase::Init(const std::string_view sceneName) {

	m_sceneName = sceneName;

	std::cout << "Loading scene: " << sceneName << std::endl;
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
void SceneBase::AddLight(const StaticDirectionalLight& light) {
	m_staticDirectionalLights.push_back(light);
}

/***********************************************************************************/
void SceneBase::AddLight(const StaticPointLight& light) {
	m_staticPointLights.push_back(light);
}

/***********************************************************************************/
void SceneBase::AddLight(const StaticSpotLight& light) {
	m_staticSpotLights.push_back(light);
}

/***********************************************************************************/
void SceneBase::AddModel(const ModelPtr& model) {
	m_sceneModels.push_back(model);
}

/***********************************************************************************/
void SceneBase::cullViewFrustum() {
	// TODO: optimize projection matrix calculation
	const ViewFrustum viewFrustum(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_width, m_height));
	m_renderList.clear();

	concurrency::parallel_for_each(m_sceneModels.cbegin(), m_sceneModels.cend(), [&](const auto& model) {
		const auto result = viewFrustum.TestIntersection(model->GetBoundingBox());

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			m_renderList.push_back(model);
		}
	});
}