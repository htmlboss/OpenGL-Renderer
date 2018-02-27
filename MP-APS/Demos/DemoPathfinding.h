#pragma once
#include "../SceneBase.h"

class DemoPathfinding : public SceneBase {
	
public:
	DemoPathfinding(const std::size_t width, const std::size_t height);
	~DemoPathfinding() = default;

	void Init(const std::string_view sceneName) override;
};