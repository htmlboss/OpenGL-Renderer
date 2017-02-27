#include "Model.h"

#include <assimp/postprocess.h>
#include <iostream>

/***********************************************************************************/
Model::Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder /*= false*/) : m_name(Name), m_path(Path) {
	
	if(!loadModel(Path, flipWindingOrder)) {
		std::cerr << "Failed to load: " << Name << '\n';
	}
}

/***********************************************************************************/
Model::~Model() {
}

/***********************************************************************************/
void Model::SetInstancing(const std::initializer_list<glm::vec3>& instanceOffsets) {
	// Pass list to each mesh
	for (auto& mesh : m_meshes) {
		mesh.SetInstancing(instanceOffsets);
	}
}

/***********************************************************************************/
void Model::Draw(GLShaderProgram& shader) {
	for (auto& mesh : m_meshes) {
		mesh.Draw(shader);
	}
}

/***********************************************************************************/
void Model::DrawInstanced(GLShaderProgram& shader) {
	for (auto& mesh : m_meshes) {
		mesh.DrawInstanced(shader);
	}
}

/***********************************************************************************/
bool Model::loadModel(const std::string_view Path, const bool flipWindingOrder) {
	std::cout << "\nLoading model: " << m_name << '\n';
	
	Assimp::Importer importer;
	const aiScene* scene = nullptr;

	if (flipWindingOrder) {
		scene = importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder | // Reverse back-face culling
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	} 
	else {
		scene = importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace | // Tangents and bitangents
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	}

	// Check if scene is not null and model is done loading
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error for " << m_name << ": " << importer.GetErrorString() << std::endl;
		return false;
	}

	m_path = Path.substr(0, Path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	std::cout << "\nLoaded Model: " << m_name << '\n';
	return true;
}

/***********************************************************************************/
void Model::processNode(aiNode* node, const aiScene* scene) {

	// Process all node meshes
	for (GLuint i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene));
	}

	// Process their children
	for (GLuint i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

/***********************************************************************************/
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		if (mesh->HasPositions()) {
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
		}
		
		if (mesh->HasNormals()) {
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}
		
		if (mesh->HasTangentsAndBitangents()) {
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			vertex.Bitangent.x = mesh->mBitangents[i].x;
			vertex.Bitangent.y = mesh->mBitangents[i].y;
			vertex.Bitangent.z = mesh->mBitangents[i].z;
		}

		if (mesh->HasTextureCoords(0)) {
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		} else {
			vertex.TexCoords = glm::vec2(0.0f);
		}
		vertices.push_back(vertex);
	}

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0) {
		auto* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		const auto diffuseMaps = loadMatTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		// 2. Specular maps
		const auto specularMaps = loadMatTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		/*
		// 3. Reflectance maps
		const auto reflectanceMaps = loadMatTextures(material, aiTextureType_AMBIENT, "texture_reflectance");
		textures.insert(textures.end(), reflectanceMaps.begin(), reflectanceMaps.end());

		// 4. Normal maps
		const auto normalMaps = loadMatTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		*/
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

/***********************************************************************************/
std::vector<Texture> Model::loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName) {
	
	std::vector<Texture> textures;

	// Get all textures
	for (GLuint c = 0; c < mat->GetTextureCount(type); ++c) {
		aiString texturePath;
		mat->GetTexture(type, c, &texturePath);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (auto& loadedTex : m_loadedTextures) {

			if (std::strcmp(loadedTex.GetRelativePath().c_str(), texturePath.C_Str()) == 0) {
				textures.push_back(loadedTex);
				skip = true;
				break;
			}
		}

		if (!skip) {   // If texture hasn't been loaded already, load it
			
			const auto texDirPrefix = m_path + "/"; // Get directory path and append forward-slash
			Texture texture(texDirPrefix, texturePath.C_Str(), samplerName, Texture::WrapMode::REPEAT);
			
			std::cout << "Texture not found! Adding: " << texDirPrefix + texturePath.C_Str() << '\n';

			textures.push_back(texture);
			m_loadedTextures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}
