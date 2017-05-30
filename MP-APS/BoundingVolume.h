#pragma once

#include <glm/vec3.hpp>

#include <memory>

class BoundingBox;
class BoundingSphere;

class BoundingVolume {
public:

	BoundingVolume() noexcept {}
	virtual ~BoundingVolume() = default;

	enum class TestResult {
		OUTSIDE,
		INTERSECT,
		INSIDE
	};

	virtual TestResult TestIntersection(const glm::vec3& point) const = 0;
	virtual TestResult TestIntersection(const std::shared_ptr<const BoundingBox> box) const = 0;
	virtual TestResult TestIntersection(const std::shared_ptr<const BoundingSphere> sphere) const = 0;
};

