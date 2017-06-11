#pragma once

#include "Camera.h"
#include "Model.h"
#include "Terrain.h"


class Scene {
public:
	Scene(const size_t width, const size_t height) noexcept;


	void Init();
	void Update(const double delta);

	auto GetCamera() const noexcept { return m_camera; }
	auto GetTerrain() const noexcept { return m_terrain; }
	auto GetRenderList() const noexcept { return m_renderList; }

private:
	size_t m_width, m_height;

	Camera m_camera;
	Terrain m_terrain;
	std::vector<ModelPtr> m_sceneModels;
	std::vector<ModelPtr> m_renderList;
};
