#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "BoundingBox.h"

#include <string_view>

class Model {
public:
	Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder = false);
	Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) noexcept;
	explicit Model(const Mesh& mesh) noexcept;

	void SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets);

	void Draw(GLShaderProgram* shader);
	void DrawInstanced(GLShaderProgram* shader);

	// Transformations
	void Scale(const glm::vec3& scale) noexcept { m_scale = scale; }
	void Rotate(const float radians, const glm::vec3& axis) noexcept { m_radians = radians; m_axis = axis; }
	void Translate(const glm::vec3& pos) noexcept { m_position = pos; }
	// Build model matrix from stored transformation data
	glm::mat4 GetModelMatrix() const noexcept;

	auto GetBoundingBox() const noexcept { return m_aabb; }

private:
	bool loadModel(const std::string_view Path, const bool flipWindingOrder);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<GLTexture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName);

	glm::vec3 m_scale, m_position, m_axis;
	float m_radians;

	BoundingBox m_aabb;

	std::vector<Mesh> m_meshes;
	std::vector<GLTexture> m_loadedTextures;

	const std::string m_name;
	std::string m_path;


};

using ModelPtr = std::unique_ptr<Model>;