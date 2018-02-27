#pragma once

#include "Camera.h"
#include "Model.h"

#include "Graphics/StaticDirectionalLight.h"
#include "Graphics/StaticPointLight.h"
#include "Graphics/StaticSpotLight.h"

/***********************************************************************************/
// Forward Declarations
class RenderSystem;

/***********************************************************************************/
class SceneBase {
	friend class RenderSystem;
public:
	SceneBase(const std::size_t width, const std::size_t height);
	virtual ~SceneBase() = default;

	virtual void Init(const std::string_view sceneName);
	virtual void Update(const double delta);

	auto GetName() const noexcept { return m_sceneName; }
	auto GetCamera() const noexcept { return m_camera; }

protected:
	// Frustum culling (and repopulate render list to be sent to renderer)
	void cullViewFrustum();

	std::string m_sceneName;

	std::size_t m_width, m_height;

	Camera m_camera;

	std::vector<StaticDirectionalLight> m_staticDirectionalLights;
	std::vector<StaticPointLight> m_staticPointLights;
	std::vector<StaticSpotLight> m_staticSpotLights;

	std::vector<ModelPtr> m_sceneModels;
	std::vector<ModelPtr> m_renderList;
};
