#include "Terrain.h"
#include "ResourceLoader.h"

/***********************************************************************************/
Terrain::Terrain() : m_heightScale(12.0f) {

	const auto data = ResourceLoader::LoadSTBImage("", &m_width, &m_length, nullptr, ResourceLoader::GREY);
	m_heightMap.reserve(m_width * m_length * sizeof(float));
	m_normals.reserve(m_width * m_length * sizeof(float) * 3);

	// Fill data
	int aux;
	float height;
	for (size_t i = 0; i < m_length; i++) {
		for (size_t j = 0; j < m_width; j++) {
			
			aux = (i * m_width) + j;
			height = data[aux] / 256.0f;
			m_heightMap.at((i * m_width) + j) = height;
		}
	}

	calculateNormals();

	// Cleanup
	delete[] data;
}

/***********************************************************************************/
Terrain::~Terrain() {
}

void Terrain::calculateNormals() {
}
