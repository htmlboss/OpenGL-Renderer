#include "Terrain.h"
#include "ResourceManager.h"

#include <future>

/***********************************************************************************/
Terrain::Terrain(const std::size_t gridX, const std::size_t gridZ) : m_x(gridX * SIZE), m_z(gridZ * SIZE) {
	std::cout << "Generating terrain.\n";

	m_terrainModel = std::make_unique<Model>(this->generateTerrain());
	m_terrainModel->Scale(glm::vec3(1.0f));
}

/***********************************************************************************/
Mesh Terrain::generateTerrain() const {

	m_heightmap = ResourceManager::GetInstance().GetTexture("textures/heightmap.png", ResourceManager::ColorMode::GREY);
	VERTEX_COUNT = m_heightmap->GetHeight();

	auto f1 = std::async(std::launch::async, &Terrain::generateVertices, this);
	auto f2 = std::async(std::launch::async, &Terrain::calculateIndices, this);

	return Mesh(f1.get(), f2.get(), loadTextures());
}

/***********************************************************************************/
std::vector<Vertex> Terrain::generateVertices() const {
	std::vector<Vertex> vertices;

	// Generate vertices
	for (auto i = 0; i < VERTEX_COUNT; ++i) {
		for (auto j = 0; j < VERTEX_COUNT; ++j) {
			
			vertices.emplace_back(Vertex(	{	static_cast<float>(j) / static_cast<float>(VERTEX_COUNT - 1) * SIZE,	// X
												0.0f,																	// Y
												static_cast<float>(i) / static_cast<float>((VERTEX_COUNT - 1)) * SIZE	// Z
											},

											{static_cast<float>(j) / static_cast<float>(VERTEX_COUNT - 1),			// U
											static_cast<float>(i) / static_cast<float>(VERTEX_COUNT - 1)},			// V
											{0.0f, 1.0f, 0.0f} ));	// Normals (x, y, z)
		}
	}

	return vertices;
}

/***********************************************************************************/
std::vector<GLuint> Terrain::calculateIndices() const {
	std::vector<GLuint> indices;

	for (auto gz = 0; gz < VERTEX_COUNT - 1; ++gz) {
		for (auto gx = 0; gx < VERTEX_COUNT - 1; ++gx) {
			
			const GLuint topLeft = (gz * VERTEX_COUNT) + gx;
			const GLuint topRight = topLeft + 1;
			const GLuint bottomLeft = (gz + 1) * VERTEX_COUNT + gx;
			const GLuint bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	return indices;
}

/***********************************************************************************/
std::vector<GLTexture> Terrain::loadTextures() const {
	std::vector<GLTexture> textures;

	textures.emplace_back(GLTexture("", "textures/grass.jpg", "texture_diffuse1"));
	textures.emplace_back(GLTexture("", "textures/flowers.jpg", "texture_diffuse2"));
	textures.emplace_back(GLTexture("", "textures/soil.jpg", "texture_diffuse3"));
	textures.emplace_back(GLTexture("", "textures/gravel.jpg", "texture_diffuse4"));
	textures.emplace_back(GLTexture("", "textures/terrainBlendMap.png", "texture_diffuse5"));

	return textures;
}

/***********************************************************************************/
float Terrain::generateHeight(const int x, int z) const {
	// Are we inside heightmap
	if (x < 0 || x >= m_heightmap->GetHeight() || z < 0 || z >= m_heightmap->GetHeight()) {
		return 0.0f;
	}

	float height = m_heightmap->GetData()[z, x];
	
	std::cout << height << '\n';

	return height;
}
