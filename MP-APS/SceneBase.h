#pragma once
#include "Utils.h"

#include "Model.h"

#include "Graphics/StaticDirectionalLight.h"
#include "Graphics/StaticPointLight.h"
#include "Graphics/StaticSpotLight.h"

/***********************************************************************************/
// Forward Declarations
class Engine;
class RenderSystem;

/***********************************************************************************/
class SceneBase {
	friend class Engine;
	friend class RenderSystem;
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;

	MAKE_MOVE_ONLY(SceneBase)

	virtual void Init(const std::string_view sceneName);
	virtual void Update(const double dt);

	auto GetName() const noexcept { return m_sceneName; }

protected:
	void AddLight(const StaticDirectionalLight& light);
	void AddLight(const StaticPointLight& light);
	void AddLight(const StaticSpotLight& light);

	void AddModel(const ModelPtr& model);

private:
	std::string m_sceneName;
	std::string m_skyboxPath = "Data/hdri/barcelona.hdr";
	std::size_t m_skyboxResolution = 2048;

	std::vector<StaticDirectionalLight> m_staticDirectionalLights;
	std::vector<StaticPointLight> m_staticPointLights;
	std::vector<StaticSpotLight> m_staticSpotLights;

	std::vector<ModelPtr> m_sceneModels;
};

