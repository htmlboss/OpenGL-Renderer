#pragma once

#include "PointLight.h"
#include "DirectionalLight.h"

#include <vector>

class RenderSystem;

// Handles all the lights in a given scene.
class LightCollection {
	friend class RenderSystem;
public:
	LightCollection() = default;

private:
	std::vector<PointLight> m_pointLights;
	std::vector<DirectionalLight> m_directionalLights;
};