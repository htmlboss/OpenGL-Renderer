#pragma once

#include "Camera.h"
#include "Model.h"

#include "Graphics/PointLight.h"
#include "Graphics/DirectionalLight.h"
#include "Graphics/LightCollection.h"

class RenderSystem;
/***********************************************************************************/
class SceneBase {
	friend class RenderSystem;
public:
	SceneBase(const std::size_t width, const std::size_t height);
	virtual ~SceneBase() = default;

	virtual void Init();
	virtual void Update(const double delta);

	auto GetCamera() const noexcept { return m_camera; }

private:
	// Frustum culling (and repopulate render list to be sent to renderer)
	void cullViewFrustum();

	std::size_t m_width, m_height;

	Camera m_camera;

	LightCollection m_lightCollection;

	DirectionalLight m_sun;
	std::vector<PointLight> m_lights;
	std::vector<ModelPtr> m_sceneModels;
	std::vector<ModelPtr> m_renderList;
};
