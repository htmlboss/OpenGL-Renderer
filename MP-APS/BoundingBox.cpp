#include "BoundingBox.h"


/***********************************************************************************/
BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& pos) noexcept :	BoundingVolume(), 
																										minimum(min), 
																										maximum(max), 
																										position(pos) {
}

/***********************************************************************************/
glm::vec3 BoundingBox::GetPositiveVertex(const glm::vec3& normal) const {
	auto positiveVertex = minimum;

	if (normal.x >= 0.0f) positiveVertex.x = maximum.x;
	if (normal.y >= 0.0f) positiveVertex.y = maximum.y;
	if (normal.z >= 0.0f) positiveVertex.z = maximum.z;

	return position + positiveVertex;
}

/***********************************************************************************/
glm::vec3 BoundingBox::GetNegativeVertex(const glm::vec3& normal) const {
	auto negativeVertex = maximum;
 
	if (normal.x >= 0.0f) negativeVertex.x = minimum.x;
	if (normal.y >= 0.0f) negativeVertex.y = minimum.y;
	if (normal.z >= 0.0f) negativeVertex.z = minimum.z;
 
	return position + negativeVertex;
}

/***********************************************************************************/
BoundingVolume::TestResult BoundingBox::TestIntersection(const glm::vec3& point) const {
	return TestResult();
}

/***********************************************************************************/
BoundingVolume::TestResult BoundingBox::TestIntersection(const std::shared_ptr<const BoundingBox> box) const {
	return TestResult();
}

/***********************************************************************************/
BoundingVolume::TestResult BoundingBox::TestIntersection(const std::shared_ptr<const BoundingSphere> sphere) const {
	return TestResult();
}