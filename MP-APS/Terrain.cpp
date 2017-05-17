#include "Terrain.h"
#include "HeightGenerator.h"

#include <future>
#include <iostream>

/***********************************************************************************/
Terrain::Terrain(const std::size_t gridX, const std::size_t gridZ, const std::size_t seed) noexcept : m_seed(seed) {
	std::cout << "Generating terrain.\n";

	m_terrainModel = std::make_unique<Model>(this->generateTerrain(gridX, gridZ));
	m_terrainModel->Translate(glm::vec3(gridX * SIZE, 0.0f, gridZ * SIZE));
	m_terrainModel->Scale(glm::vec3(1.0f));
}

/***********************************************************************************/
Mesh Terrain::generateTerrain(const std::size_t x, const std::size_t z) {
	VERTEX_COUNT = 128;

	HeightGenerator heightGenerator(x, z, VERTEX_COUNT, m_seed);
	
	for (auto i = 0; i < VERTEX_COUNT; ++i) {
		std::vector<float> rows;
		for (auto j = 0; j < VERTEX_COUNT; ++j) {
			rows.push_back(heightGenerator.GenerateHeight(j, i));
		}
		m_heightData.push_back(rows);
	}

	auto f1 = std::async(std::launch::async, &Terrain::generateVertices, this, heightGenerator);
	auto f2 = std::async(std::launch::async, &Terrain::calculateIndices, this);

	return Mesh(f1.get(), f2.get(), loadTextures());
}

/***********************************************************************************/
std::vector<Vertex> Terrain::generateVertices(const HeightGenerator& generator) const {
	std::vector<Vertex> vertices;
	
	// Generate vertices
	for (auto i = 0; i < VERTEX_COUNT; ++i) {
		for (auto j = 0; j < VERTEX_COUNT; ++j) {

			vertices.emplace_back(Vertex(	{	static_cast<float>(j) / static_cast<float>(VERTEX_COUNT - 1) * SIZE,	// X
												m_heightData[j][i],														// Y
												static_cast<float>(i) / static_cast<float>((VERTEX_COUNT - 1)) * SIZE	// Z
											},

											{	static_cast<float>(j) / static_cast<float>(VERTEX_COUNT - 1),			// U
												static_cast<float>(i) / static_cast<float>(VERTEX_COUNT - 1)},			// V
												
												calculateNormal(j, i, generator)));
		}
	}

	return vertices;
}

/***********************************************************************************/
std::vector<GLuint> Terrain::calculateIndices() const noexcept {
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
	textures.emplace_back(GLTexture("", "textures/forest_trail/forest-trail1-albedo.png", "texture_diffuse3"));
	textures.emplace_back(GLTexture("", "textures/gravel.jpg", "texture_diffuse4"));
	textures.emplace_back(GLTexture("", "textures/terrainBlendMap.png", "texture_diffuse5"));

	return textures;
}

/***********************************************************************************/
glm::vec3 Terrain::calculateNormal(const int x, const int z, const HeightGenerator& generator) const {

	float heightL, heightR, heightU, heightD;
	// Are we at the boundary of the heightData?
	if (x == 0 || x == VERTEX_COUNT-1 || z == 0 || z == VERTEX_COUNT-1) {
		heightL = generator.GenerateHeight(x - 1, z);
		heightR = generator.GenerateHeight(x + 1, z);
		heightU = generator.GenerateHeight(x, z + 1);
		heightD = generator.GenerateHeight(x, z - 1);
	}
	else {
		// get data from heightData
		heightL = m_heightData[x - 1][z];
		heightR = m_heightData[x + 1][z];
		heightU = m_heightData[x][z + 1];
		heightD = m_heightData[x][z - 1];	
	}

	return glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
}
