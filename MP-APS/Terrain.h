#pragma once

#include <vector>

class Terrain {
public:
	Terrain();
	~Terrain();

private:
	void calculateNormals();

	int m_width, m_length;
	const float m_heightScale;

	std::vector<float> m_heightMap;
	std::vector<float> m_normals;
};

