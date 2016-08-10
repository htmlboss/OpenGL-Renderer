#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model {
public:
	Model(const std::string& Path);
	~Model();

	void Draw(const Shader& shader);

private:
	void loadModel(const std::string& Path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string& samplerName);

	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_loadedTextures;
	std::string m_directory;
};

