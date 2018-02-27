#pragma once

#include "../SceneBase.h"

class DemoCrytekSponza : public SceneBase {

public:
	DemoCrytekSponza(const std::size_t width, const std::size_t height);
	~DemoCrytekSponza() = default;
	
	void Init(const std::string_view sceneName) override;
};
