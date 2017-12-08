#include "Model.h"
#include "Core/RenderSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ppl.h>

/***********************************************************************************/
Model::Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder, const bool loadTextures) : m_name(Name), m_path(Path) {
	
	if (!loadModel(Path, flipWindingOrder, loadTextures)) {
		std::cerr << "Failed to load: " << Name << '\n';
	}
	m_loadedTextures.clear();
}

/***********************************************************************************/
Model::Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) noexcept { 
	m_meshes.emplace_back(vertices, indices, textures); 
}

/***********************************************************************************/
Model::Model(const std::string_view Name, const Mesh& mesh) noexcept : m_name(Name) {
	m_meshes.push_back(std::move(mesh));
}

/***********************************************************************************/
void Model::AssignMaterial(const Material& material) {
	m_material = material;
}

/***********************************************************************************/
void Model::AttachMesh(const Mesh& mesh) noexcept {
	m_meshes.push_back(mesh);
}

/***********************************************************************************/
void Model::Scale(const glm::vec3& scale) noexcept {
	m_scale = scale;
	m_aabb.scale(scale, glm::vec3(0.0f));
}

/***********************************************************************************/
void Model::Rotate(const float radians, const glm::vec3& axis) noexcept {
	m_radians = radians;
	m_axis = axis;
}

/***********************************************************************************/
void Model::Translate(const glm::vec3& pos) noexcept {
	m_position = pos;
	m_aabb.translate(pos);
}

/***********************************************************************************/
glm::mat4 Model::GetModelMatrix() const noexcept {

	const auto scale = glm::scale(glm::mat4(1.0f), m_scale);
	const auto translate = glm::translate(glm::mat4(1.0f), m_position);

	return scale * translate;
}

/***********************************************************************************/
bool Model::loadModel(const std::string_view Path, const bool flipWindingOrder, const bool loadTextures = true) {
#ifdef _DEBUG
	std::cout << "Loading model: " << m_name << '\n';
#endif

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
		                          aiProcess_CalcTangentSpace |
		                          aiProcess_GenSmoothNormals |
		                          aiProcess_ImproveCacheLocality |
		                          aiProcess_OptimizeMeshes |
		                          aiProcess_SplitLargeMeshes);
	}

	// Check if scene is not null and model is done loading
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error for " << m_name << ": " << importer.GetErrorString() << '\n';
		importer.FreeScene();

		return false;
	}

	m_path = Path.substr(0, Path.find_last_of('/'));
	processNode(scene->mRootNode, scene, loadTextures);

	importer.FreeScene();
	return true;
}

/***********************************************************************************/
void Model::processNode(aiNode* node, const aiScene* scene, const bool loadTextures) {

	// Process all node meshes
	concurrency::parallel_for(static_cast<unsigned int>(0), node->mNumMeshes, [&](const auto idx)
	{
		auto* mesh = scene->mMeshes[node->mMeshes[idx]];
		m_meshes.push_back(processMesh(mesh, scene, loadTextures));
	});

	// Process their children
	for (auto i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, loadTextures);
	}
}

/***********************************************************************************/
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const bool loadTextures) {
	std::vector<Vertex> vertices;
	std::vector<GLTexture> textures;
	glm::vec3 min, max;

	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		if (mesh->HasPositions()) {
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			// Construct bounding box
			if (vertex.Position.x < min.x) min.x = vertex.Position.x;
			if (vertex.Position.x > max.x) max.x = vertex.Position.x;

			if (vertex.Position.y < min.y) min.y = vertex.Position.y;
			if (vertex.Position.y > max.y) max.y = vertex.Position.y;

			if (vertex.Position.z < min.z) min.z = vertex.Position.z;
			if (vertex.Position.z > max.z) max.z = vertex.Position.z;

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

		if (mesh->HasTextureCoords(0) && loadTextures) {
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else { vertex.TexCoords = glm::vec2(0.0f); }
		vertices.push_back(vertex);
	}

	m_aabb.extend(min);
	m_aabb.extend(max);

	// Get indices from each face
	std::vector<GLuint> indices;
	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		const auto face = mesh->mFaces[i];

		for (size_t j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	// Process material
	if (loadTextures) {
		if (mesh->mMaterialIndex >= 0) {
			auto* material = scene->mMaterials[mesh->mMaterialIndex];

			// PBR textures
			const auto albedoMaps = loadMatTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

			const auto specularMaps = loadMatTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			const auto normalMaps = loadMatTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}
	}
	return Mesh(vertices, indices, textures);
}

/***********************************************************************************/
std::vector<GLTexture> Model::loadMatTextures(aiMaterial* mat, aiTextureType type, const std::string_view samplerName) {

	std::vector<GLTexture> textures;

	// Get all textures
	for (size_t c = 0; c < mat->GetTextureCount(type); ++c) {
		aiString texturePath;
		mat->GetTexture(type, c, &texturePath);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (auto& loadedTex : m_loadedTextures) {

			if (strcmp(loadedTex.GetRelativePath().c_str(), texturePath.C_Str()) == 0) {
				textures.push_back(loadedTex);
				skip = true;
				break;
			}
		}

		if (!skip) { // If texture hasn't been loaded already, load it

			const auto texDirPrefix = m_path + "/"; // Get directory path and append forward-slash
			GLTexture texture(texDirPrefix, texturePath.C_Str(), samplerName, GLTexture::WrapMode::REPEAT);

			//std::cout << "GLTexture not found! Adding: " << texDirPrefix + texturePath.C_Str() << '\n';

			textures.push_back(texture);
			m_loadedTextures.push_back(texture); // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}
