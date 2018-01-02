// https://github.com/iauns/cpm-glm-aabb

#ifndef IAUNS_GLM_AABB_HPP
#define IAUNS_GLM_AABB_HPP

#include <glm/vec3.hpp>
#include <glm/detail/type_float.hpp>

/// Standalone axis aligned bounding box implemented built on top of GLM.
class AABB {
public:
	/// Builds a null AABB.
	AABB();

	/// Builds an AABB that encompasses a sphere.
	/// \param[in]  center Center of the sphere.
	/// \param[in]  radius Radius of the sphere.
	AABB(const glm::vec3& center, const glm::float_t radius);

	/// Builds an AABB that contains the two points.
	AABB(const glm::vec3& p1, const glm::vec3& p2);

	AABB(const AABB& aabb);

	/// Set the AABB as NULL (not set).
	void setNull() {
		mMin = glm::vec3(1.0f);
		mMax = glm::vec3(-1.0f);
	}

	/// Returns true if AABB is NULL (not set).
	auto isNull() const noexcept {
		return mMin.x > mMax.x || mMin.y > mMax.y || mMin.z > mMax.z;
	}

	/// Extend the bounding box on all sides by \p val.
	void extend(const glm::float_t val);

	/// Expand the AABB to include point \p p.
	void extend(const glm::vec3& p);

	/// Expand the AABB to include a sphere centered at \p center and of radius \p
	/// radius.
	/// \param[in]  center Center of sphere.
	/// \param[in]  radius Radius of sphere.
	void extend(const glm::vec3& center, glm::float_t radius);

	/// Expand the AABB to encompass the given \p aabb.
	void extend(const AABB& aabb);

	/// Expand the AABB to include a disk centered at \p center, with normal \p
	/// normal, and radius \p radius.
	/// \xxx Untested -- This function is not represented in our unit tests.
	void extendDisk(const glm::vec3& center, const glm::vec3& normal, glm::float_t radius);

	/// Translates AABB by vector \p v.
	void translate(const glm::vec3& v);

	/// Scale the AABB by \p scale, centered around \p origin.
	/// \param[in]  scale  3D vector specifying scale along each axis.
	/// \param[in]  origin Origin of scaling operation. Most useful origin would
	///                    be the center of the AABB.
	void scale(const glm::vec3& scale, const glm::vec3& origin);

	/// Retrieves the center of the AABB.
	glm::vec3 getCenter() const;

	/// Retrieves the diagonal vector (computed as mMax - mMin).
	/// If the AABB is NULL, then a vector of all zeros is returned.
	glm::vec3 getDiagonal() const;

	/// Retrieves the longest edge.
	/// If the AABB is NULL, then 0 is returned.
	glm::float_t getLongestEdge() const;

	/// Retrieves the shortest edge.
	/// If the AABB is NULL, then 0 is returned.
	glm::float_t getShortestEdge() const;

	/// Retrieves the AABB's minimum point.
	auto getMin() const {
		return mMin;
	}

	/// Retrieves the AABB's maximum point.
	auto getMax() const {
		return mMax;
	}

	/// Returns true if AABBs share a face overlap.
	/// \xxx Untested -- This function is not represented in our unit tests.
	bool overlaps(const AABB& bb) const;

	/// Type returned from call to intersect.
	enum INTERSECTION_TYPE { INSIDE, INTERSECT, OUTSIDE };

	/// Returns one of the intersection types. If either of the aabbs are invalid,
	/// then OUTSIDE is returned.
	INTERSECTION_TYPE intersect(const AABB& bb) const;

	/// Function from SCIRun. Here is a summary of SCIRun's description:
	/// Returns true if the two AABB's are similar. If diff is 1.0, the two
	/// bboxes have to have about 50% overlap each for x,y,z. If diff is 0.0,
	/// they have to have 100% overlap.
	/// If either of the two AABBs is NULL, then false is returned.
	/// \xxx Untested -- This function is not represented in our unit tests.
	bool isSimilarTo(const AABB& b, glm::float_t diff = 0.5) const;

private:

	glm::vec3 mMin; ///< Minimum point.
	glm::vec3 mMax; ///< Maximum point.
};

#endif
