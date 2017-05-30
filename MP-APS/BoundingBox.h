#pragma once
#include "BoundingVolume.h"

class BoundingBox : public BoundingVolume {
public:
	BoundingBox() noexcept {}
	BoundingBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& pos) noexcept;

	glm::vec3 GetPositiveVertex(const glm::vec3& normal) const;
	glm::vec3 GetNegativeVertex(const glm::vec3& normal) const;

	TestResult TestIntersection(const glm::vec3& point) const override;
	TestResult TestIntersection(const std::shared_ptr<const BoundingBox> box) const override;
	TestResult TestIntersection(const std::shared_ptr<const BoundingSphere> sphere) const override;

	glm::vec3 minimum, maximum, position;
};

