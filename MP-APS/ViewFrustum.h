#pragma once
#include "BoundingVolume.h"

#include <glm/mat4x4.hpp>

#include <array>

// Stupid win32 junk
#ifdef FAR
	#undef FAR
#endif
#ifdef NEAR
	#undef NEAR
#endif

class AABB;

class ViewFrustum : BoundingVolume {
public:
	ViewFrustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

	auto GetPlane(const size_t plane) const noexcept {
#ifdef _DEBUG
		assert(plane < 6);
#endif
		return m_planes[plane];
	}

	enum Plane {
		FAR,
		NEAR,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	TestResult TestIntersection(const glm::vec3& point) const override;
	TestResult TestIntersection(const AABB& aabb) const;
	TestResult TestIntersection(const std::shared_ptr<const BoundingSphere> sphere) const override;

private:
	// ax + by + cz = d
	std::array<glm::vec4, 6> m_planes;
};
