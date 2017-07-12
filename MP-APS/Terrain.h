#pragma once

#include "Model.h"

class HeightGenerator;

class Terrain : public Model {
public:
	Terrain(const std::size_t gridX, const std::size_t gridZ, const std::size_t seed) noexcept;
	~Terrain() override {}


private:
	Mesh generateTerrain(const std::size_t x, const std::size_t z);
	std::vector<Vertex> generateVertices(const HeightGenerator& generator) const;
	std::vector<GLuint> calculateIndices() const noexcept;
	std::vector<GLTexture> loadTextures() const;
	glm::vec3 calculateNormal(const int x, const int y, const HeightGenerator& generator) const;

	std::vector<std::vector<float>> m_heightData;

	std::size_t VERTEX_COUNT;
	const std::size_t SIZE = 1000, MAX_HEIGHT = 75;
	const std::size_t m_seed;
};

