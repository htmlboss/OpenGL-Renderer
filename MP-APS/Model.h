#pragma once

#include <glm/mat4x4.hpp>

#include "Mesh.h"
#include "AABB.hpp"

#include <memory>

struct aiScene;
struct aiNode;
struct aiMesh;

class Model {
public:
	Model() = default;
	Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder = false, const bool loadMaterial = true);
	Model(const std::string_view Name, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const PBRMaterial& material) noexcept;
	Model(const std::string_view Name, const Mesh& mesh) noexcept;
	virtual ~Model() = default;

	void AttachMesh(const Mesh mesh) noexcept;

	// Transformations
	void Scale(const glm::vec3& scale);
	void Rotate(const float radians, const glm::vec3& axis);
	void Translate(const glm::vec3& pos);
	glm::mat4 GetModelMatrix() const;

	auto GetMeshes() const noexcept { return m_meshes; }
	auto GetBoundingBox() const noexcept { return m_aabb; }

protected:
	std::vector<Mesh> m_meshes;

private:
	bool loadModel(const std::string_view Path, const bool flipWindingOrder, const bool loadMaterial);
	void processNode(aiNode* node, const aiScene* scene, const bool loadMaterial);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const bool loadMaterial);
	
	// Transformation data
	glm::vec3 m_scale, m_position, m_axis;
	float m_radians;

	AABB m_aabb; // Model bounding box

	// Model name
	const std::string m_name;
	// Location on disk holding model and textures
	std::string m_path;

	std::size_t m_numMats;
};

using ModelPtr = std::shared_ptr<Model>;
