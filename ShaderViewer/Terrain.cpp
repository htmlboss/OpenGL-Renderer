#include "Terrain.h"
#include "ResourceLoader.h"

/***********************************************************************************/
Terrain::Terrain() : m_heightScale(12.0f) {

	const auto data = ResourceLoader::LoadSTBImage("", &m_width, &m_height, nullptr, ResourceLoader::GREY);
	m_heightMap.reserve(m_width * m_height);
	
	size_t index;
	auto k = 0;
	unsigned char heightValue;

	for (auto j = 0; j < m_height; j++) {
		for (auto i = 0; i < m_width; i++) {
			
			// Bitmaps are upside down so load bottom to top into the height map vector
			index = m_width * (m_height - 1 - j) + i;

			// Get the grey scale pixel value from the bitmap image data at this location
			heightValue = data[k];

			// Store the pixel value as the height at this point in height map
			// Note: OpenGL coordinate system is Y-UP >.>
			m_heightMap.at(index).y = static_cast<float>(heightValue);

			// Increment bitmap index
			k += 3;
		}
	}

	// Cleanup
	delete[] data;
}

/***********************************************************************************/
Terrain::~Terrain() {
}

/***********************************************************************************/
void Terrain::calcVertexCoordinates() {

	auto index = 0;

	// Loop through all the elements in the height map array and adjust their coordinates correctly
	for (auto j = 0; j < m_height; j++) {
		for (auto i = 0; i < m_width; i++) {
			
			index = (m_width * j) + i;

			// Set X and Z coordinates for OpenGL
			m_heightMap.at(index).x = static_cast<float>(i);
			m_heightMap.at(index).z = static_cast<float>(j); // May need to negate this...

			// Move terrain depth into positive range (may be optional)
			m_heightMap.at(index).z += static_cast<float>(m_height - 1.0f);

			// Scale height
			m_heightMap.at(index).y /= m_heightScale;
		}
	}
}

/***********************************************************************************/
void Terrain::buildTerrain() {

	const auto vertexCount = m_width * m_height * 6;
	m_terrainModel.reserve(vertexCount);

	auto index = 0;
	
	// Create 2 triangles for every 4 points to make a quad.
	for (auto j = 0; j < m_height; j++) {
		for (auto i = 0; i < m_width; i++) {
			
			// Calculate indices of the points
			const auto upperLeft = (m_width * j) + i;
			const auto upperRight = (m_width * j) + i + 1;
			const auto bottomLeft = (m_width * (j + 1)) + i;
			const auto bottomRight = (m_width * (j + 1)) + i + 1;


			// Consruct triangles
			// Upper left
			m_terrainModel.at(index).x = m_heightMap.at(upperLeft).x;
			m_terrainModel.at(index).y = m_heightMap.at(upperLeft).y;
			m_terrainModel.at(index).z = m_heightMap.at(upperLeft).z;
			++index;

			// Upper right
			m_terrainModel.at(index).x = m_heightMap.at(upperRight).x;
			m_terrainModel.at(index).y = m_heightMap.at(upperRight).y;
			m_terrainModel.at(index).z = m_heightMap.at(upperRight).z;
			++index;
		}
	}
}
