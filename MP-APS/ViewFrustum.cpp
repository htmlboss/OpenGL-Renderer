#include "ViewFrustum.h"

#include "AABB.hpp"

#include <glm/detail/func_geometric.hpp>


/***********************************************************************************/
ViewFrustum::ViewFrustum(const glm::mat4& v, const glm::mat4& p) {

	glm::mat4 clipMatrix;

	clipMatrix[0][0] = v[0][0] * p[0][0] + v[0][1] * p[1][0] + v[0][2] * p[2][0] + v[0][3] * p[3][0];
	clipMatrix[1][0] = v[0][0] * p[0][1] + v[0][1] * p[1][1] + v[0][2] * p[2][1] + v[0][3] * p[3][1];
	clipMatrix[2][0] = v[0][0] * p[0][2] + v[0][1] * p[1][2] + v[0][2] * p[2][2] + v[0][3] * p[3][2];
	clipMatrix[3][0] = v[0][0] * p[0][3] + v[0][1] * p[1][3] + v[0][2] * p[2][3] + v[0][3] * p[3][3];
	clipMatrix[0][1] = v[1][0] * p[0][0] + v[1][1] * p[1][0] + v[1][2] * p[2][0] + v[1][3] * p[3][0];
	clipMatrix[1][1] = v[1][0] * p[0][1] + v[1][1] * p[1][1] + v[1][2] * p[2][1] + v[1][3] * p[3][1];
	clipMatrix[2][1] = v[1][0] * p[0][2] + v[1][1] * p[1][2] + v[1][2] * p[2][2] + v[1][3] * p[3][2];
	clipMatrix[3][1] = v[1][0] * p[0][3] + v[1][1] * p[1][3] + v[1][2] * p[2][3] + v[1][3] * p[3][3];
	clipMatrix[0][2] = v[2][0] * p[0][0] + v[2][1] * p[1][0] + v[2][2] * p[2][0] + v[2][3] * p[3][0];
	clipMatrix[1][2] = v[2][0] * p[0][1] + v[2][1] * p[1][1] + v[2][2] * p[2][1] + v[2][3] * p[3][1];
	clipMatrix[2][2] = v[2][0] * p[0][2] + v[2][1] * p[1][2] + v[2][2] * p[2][2] + v[2][3] * p[3][2];
	clipMatrix[3][2] = v[2][0] * p[0][3] + v[2][1] * p[1][3] + v[2][2] * p[2][3] + v[2][3] * p[3][3];
	clipMatrix[0][3] = v[3][0] * p[0][0] + v[3][1] * p[1][0] + v[3][2] * p[2][0] + v[3][3] * p[3][0];
	clipMatrix[1][3] = v[3][0] * p[0][1] + v[3][1] * p[1][1] + v[3][2] * p[2][1] + v[3][3] * p[3][1];
	clipMatrix[2][3] = v[3][0] * p[0][2] + v[3][1] * p[1][2] + v[3][2] * p[2][2] + v[3][3] * p[3][2];
	clipMatrix[3][3] = v[3][0] * p[0][3] + v[3][1] * p[1][3] + v[3][2] * p[2][3] + v[3][3] * p[3][3];

	m_planes[RIGHT].x = clipMatrix[3][0] - clipMatrix[0][0];
	m_planes[RIGHT].y = clipMatrix[3][1] - clipMatrix[0][1];
	m_planes[RIGHT].z = clipMatrix[3][2] - clipMatrix[0][2];
	m_planes[RIGHT].w = clipMatrix[3][3] - clipMatrix[0][3];
	m_planes[RIGHT] = normalize(m_planes[RIGHT]);

	m_planes[LEFT].x = clipMatrix[3][0] + clipMatrix[0][0];
	m_planes[LEFT].y = clipMatrix[3][1] + clipMatrix[0][1];
	m_planes[LEFT].z = clipMatrix[3][2] + clipMatrix[0][2];
	m_planes[LEFT].w = clipMatrix[3][3] + clipMatrix[0][3];
	m_planes[LEFT] = normalize(m_planes[LEFT]);

	m_planes[TOP].x = clipMatrix[3][0] - clipMatrix[1][0];
	m_planes[TOP].y = clipMatrix[3][1] - clipMatrix[1][1];
	m_planes[TOP].z = clipMatrix[3][2] - clipMatrix[1][2];
	m_planes[TOP].w = clipMatrix[3][3] - clipMatrix[1][3];
	m_planes[TOP] = normalize(m_planes[TOP]);

	m_planes[BOTTOM].x = clipMatrix[3][0] + clipMatrix[1][0];
	m_planes[BOTTOM].y = clipMatrix[3][1] + clipMatrix[1][1];
	m_planes[BOTTOM].z = clipMatrix[3][2] + clipMatrix[1][2];
	m_planes[BOTTOM].w = clipMatrix[3][3] + clipMatrix[1][3];
	m_planes[BOTTOM] = normalize(m_planes[BOTTOM]);

	m_planes[NEAR].x = clipMatrix[3][0] + clipMatrix[2][0];
	m_planes[NEAR].y = clipMatrix[3][1] + clipMatrix[2][1];
	m_planes[NEAR].z = clipMatrix[3][2] + clipMatrix[2][2];
	m_planes[NEAR].w = clipMatrix[3][3] + clipMatrix[2][3];
	m_planes[NEAR] = normalize(m_planes[NEAR]);

	m_planes[FAR].x = clipMatrix[3][0] - clipMatrix[2][0];
	m_planes[FAR].y = clipMatrix[3][1] - clipMatrix[2][1];
	m_planes[FAR].z = clipMatrix[3][2] - clipMatrix[2][2];
	m_planes[FAR].w = clipMatrix[3][3] - clipMatrix[2][3];
	m_planes[FAR] = normalize(m_planes[FAR]);

}

/***********************************************************************************/
BoundingVolume::TestResult ViewFrustum::TestIntersection(const glm::vec3& point) const {
	return TestResult::INTERSECT;
}

/***********************************************************************************/
BoundingVolume::TestResult ViewFrustum::TestIntersection(const AABB& aabb) const {

	const glm::vec3 b[] {aabb.getMin(), aabb.getMax()};

	auto result = TestResult::INSIDE;

	for (auto i = 0; i < 6; ++i) {
		const auto px = m_planes[i].x > 0.0f;
		const auto py = m_planes[i].y > 0.0f;
		const auto pz = m_planes[i].z > 0.0f;

		const auto dp = m_planes[i].x * b[px].x + m_planes[i].y * b[py].y + m_planes[i].z * b[pz].z;
		const auto dp2 = m_planes[i].x * b[1 - px].x + m_planes[i].y * b[1 - py].y + m_planes[i].z * b[1 - pz].z;

		if (dp < -m_planes[i].w) { return TestResult::OUTSIDE; }

		if (dp2 <= -m_planes[i].w) { result = TestResult::INTERSECT; }
	}
	return result;
}

/***********************************************************************************/
BoundingVolume::TestResult ViewFrustum::TestIntersection(const std::shared_ptr<const BoundingSphere> sphere) const {
	return TestResult::INTERSECT;
}
