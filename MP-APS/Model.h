#pragma once

#include <assimp/scene.h>

#include "Mesh.h"
#include "AABB.hpp"
#include "Material.h"

#include <string_view>
#include <memory>

class Model {
public:
	explicit Model() noexcept : m_radians(0.0f) {}
	Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder = false);
	Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) noexcept;
	Model(const std::string_view Name, const Mesh& mesh) noexcept;
	virtual ~Model() {}

	void AssignMaterial(const Material& material);
	auto GetMaterial() const noexcept { return m_material; }

	// Transformations
	void Scale(const glm::vec3& scale) noexcept;
	void Rotate(const float radians, const glm::vec3& axis) noexcept;
	void Translate(const glm::vec3& pos) noexcept;
	glm::mat4 GetModelMatrix() const noexcept;

	auto GetMeshes() const noexcept { return m_meshes; }
	auto GetBoundingBox() const noexcept { return m_aabb; }

protected:
	std::vector<Mesh> m_meshes;

private:
	bool loadModel(const std::string_view Path, const bool flipWindingOrder);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<GLTexture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName);

	Material m_material;
	glm::vec3 m_scale, m_position, m_axis;
	float m_radians;

	AABB m_aabb; // Model bounding box

	std::vector<GLTexture> m_loadedTextures;

	const std::string m_name;
	std::string m_path;
};

using ModelPtr = std::shared_ptr<Model>;
