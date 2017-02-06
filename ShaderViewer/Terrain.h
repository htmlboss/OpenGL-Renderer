#pragma once

#include <vector>

class Terrain {
public:
	Terrain();
	~Terrain();

private:

	void calcVertexCoordinates();
	void buildTerrain();

	struct HeightMapData {
		float x, y, z;
	};

	struct ModelData {
		float x, y, z;
	};

	int m_width, m_height;
	const float m_heightScale;

	std::vector<HeightMapData> m_heightMap;
	std::vector<ModelData> m_terrainModel;
};

