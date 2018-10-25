#pragma once

#include "../SceneBase.h"

class DemoCrytekSponza : public SceneBase {

public:
	DemoCrytekSponza();
	~DemoCrytekSponza() = default;

	MAKE_MOVE_ONLY(DemoCrytekSponza)
	
	void Init(const std::string_view sceneName) override;
};
