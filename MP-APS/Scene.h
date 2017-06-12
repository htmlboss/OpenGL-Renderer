#pragma once

#include "Camera.h"
#include "Model.h"
#include "Terrain.h"
#include "Light.h"

/***********************************************************************************/
// Data that is sent to the Renderer.
struct RenderData {
	RenderData(const size_t terrainPosX, const size_t terrainPosZ) : terrain(terrainPosX, terrainPosZ, 10) {}

	Light Sun;
	std::vector<Light> lights;
	std::vector<ModelPtr> renderList;
	Terrain terrain;
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
	RenderData m_renderData;

	size_t m_width, m_height;

	Camera m_camera;
	
	std::vector<ModelPtr> m_sceneModels;
};
