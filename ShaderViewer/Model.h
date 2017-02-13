#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"

class Model {
public:
	Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder = false);
	~Model();

	void SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets);

	void Draw(GLShaderProgram& shader);
	void DrawInstanced(GLShaderProgram& shader);

private:
	bool loadModel(const std::string_view Path, const bool flipWindingOrder);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string& samplerName);

	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_loadedTextures;

	const std::string m_name;
	std::string m_path;
};

