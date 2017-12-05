#pragma once

#include "Mesh.h"

class MeshBuilder {
	MeshBuilder() noexcept = default;
	~MeshBuilder() noexcept = default;
public:

	static auto& GetInstance() {
		static MeshBuilder instance;
		return instance;
	}

	// Various mesh types for a sphere:
	// https://gamedevdaily.io/four-ways-to-create-a-mesh-for-a-sphere-d7956b825db4
	Mesh CreateUVSphere(const std::size_t latitudeCount = 32, const std::size_t longtitudeCount = 32) const;
	Mesh CreateNormalizedCube() const;
	Mesh CreateSpherifiedCube() const;
	Mesh CreateIcosahedron() const;

	Mesh CreateCube() const;
	Mesh CreatePlane() const;
};

