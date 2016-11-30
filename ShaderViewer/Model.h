#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"

class Model {
public:
	Model(const std::string& Path, const std::string& Name);
	~Model();

	void SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets);

	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader);

private:
	bool loadModel(const std::string& Path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string& samplerName);

	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_loadedTextures;

	const std::string m_name;
	std::string m_path;
};

