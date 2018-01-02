#include "Model.h"
#include "Core/RenderSystem.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ppl.h>

/***********************************************************************************/
Model::Model(const std::string_view Path, const std::string_view Name, const bool flipWindingOrder, const bool loadMaterial) : m_name(Name), m_path(Path) {
	
	if (!loadModel(Path, flipWindingOrder, loadMaterial)) {
		std::cerr << "Failed to load: " << Name << '\n';
	}
}

/***********************************************************************************/
Model::Model(const std::string_view Name, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const PBRMaterial& material) noexcept : m_name(Name) { 
	m_meshes.emplace_back(vertices, indices, material); 
}

/***********************************************************************************/
Model::Model(const std::string_view Name, const Mesh& mesh) noexcept : m_name(Name) {
	m_meshes.push_back(std::move(mesh));
}

/***********************************************************************************/
void Model::AttachMesh(const Mesh mesh) noexcept {
	m_meshes.push_back(mesh);
}

/***********************************************************************************/
void Model::Scale(const glm::vec3& scale) {
	m_scale = scale;
	m_aabb.scale(scale, glm::vec3(0.0f));
}

/***********************************************************************************/
void Model::Rotate(const float radians, const glm::vec3& axis) {
	m_radians = radians;
	m_axis = axis;
}

/***********************************************************************************/
void Model::Translate(const glm::vec3& pos) {
	m_position = pos;
	m_aabb.translate(pos);
}

/***********************************************************************************/
glm::mat4 Model::GetModelMatrix() const {

	const auto scale = glm::scale(glm::mat4(1.0f), m_scale);
	const auto translate = glm::translate(glm::mat4(1.0f), m_position);

	return scale * translate;
}

/***********************************************************************************/
bool Model::loadModel(const std::string_view Path, const bool flipWindingOrder, const bool loadMaterial = true) {
#ifdef _DEBUG
	std::cout << "Loading model: " << m_name << '\n';
#endif

	std::cout << sizeof(PBRMaterial) << '\n';

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

	m_path = Path.substr(0, Path.find_last_of('/')); // Strip the model file name and keep the model folder.
	m_path += "/";
	
	processNode(scene->mRootNode, scene, loadMaterial);

	importer.FreeScene();
	return true;
}

/***********************************************************************************/
void Model::processNode(aiNode* node, const aiScene* scene, const bool loadMaterial) {

	// Process all node meshes
	concurrency::parallel_for(static_cast<unsigned int>(0), node->mNumMeshes, [&](const auto idx)
	{
		auto* mesh = scene->mMeshes[node->mMeshes[idx]];
		m_meshes.push_back(processMesh(mesh, scene, loadMaterial));
	});

	// Process their children via recursive tree traversal
	for (auto i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, loadMaterial);
	}
}

/***********************************************************************************/
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const bool loadMaterial) {
	std::vector<Vertex> vertices;
	glm::vec3 min, max;

	for (auto i = 0; i < mesh->mNumVertices; ++i) {
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
		}

		if (mesh->HasTextureCoords(0) && loadMaterial) {
			// Just take the first set of texture coords (since we could have up to 8)
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		} else { 
			vertex.TexCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// Resize the bounding box
	m_aabb.extend(min);
	m_aabb.extend(max);

	// Get indices from each face
	std::vector<GLuint> indices;
	for (auto i = 0; i < mesh->mNumFaces; ++i) {
		const auto face = mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	// Process material
	// http://assimp.sourceforge.net/lib_html/structai_material.html
	if (loadMaterial) {
		if (mesh->mMaterialIndex >= 0) {
			PBRMaterial material;
			const auto* mat = scene->mMaterials[mesh->mMaterialIndex];

			aiString name;
			mat->Get(AI_MATKEY_NAME, name);

			// Get the first texture for each texture type we need
			// since there could be multiple textures per type
			aiString albedoPath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &albedoPath);

			aiString metallicPath;
			mat->GetTexture(aiTextureType_AMBIENT, 0, &metallicPath);

			aiString normalPath;
			mat->GetTexture(aiTextureType_HEIGHT, 0, &normalPath);

			aiString roughnessPath;
			mat->GetTexture(aiTextureType_SHININESS, 0, &roughnessPath);

			aiString alphaMaskPath;
			mat->GetTexture(aiTextureType_OPACITY, 0, &alphaMaskPath);

			std::cout << "MATERIAL NAME: " << name.C_Str() << std::endl;

			material.Init(name.C_Str(),
				m_path + albedoPath.C_Str(),
				"",
				m_path + metallicPath.C_Str(),
				m_path + normalPath.C_Str(),
				m_path + roughnessPath.C_Str(),
				m_path + alphaMaskPath.C_Str()
			);

			++m_numMats;
			std::cout << "NUMBER OF MATERIALS: " << m_numMats << std::endl;
			return Mesh(vertices, indices, material);
		}
	}

	return Mesh(vertices, indices);
}
