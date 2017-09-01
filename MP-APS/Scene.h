#pragma once

#include "Camera.h"
#include "Model.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"

/***********************************************************************************/
// Data that is sent to the Renderer.
struct RenderData {

	DirectionalLight Sun;
	std::vector<PointLight> lights;
	std::vector<ModelPtr> renderList;
};

/***********************************************************************************/
class Scene {
public:
	Scene(const size_t width, const size_t height);

	void Init();
	void Update(const double delta);

	auto GetCamera() const noexcept { return m_camera; }
	auto GetRenderData() const noexcept { return m_renderData; }

private:
	// Frustum culling (and repopulate render list to be sent to renderer)
	void cullViewFrustum();
	
	RenderData m_renderData;

	size_t m_width, m_height;

	Camera m_camera;
	
	std::vector<ModelPtr> m_sceneModels;
};
