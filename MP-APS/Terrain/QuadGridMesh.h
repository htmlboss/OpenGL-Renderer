#pragma once
#include "GridMesh.h"

class QuadGridMesh {

public:
	// Specify the size of the 4 subquads together, not the size of one subquad
	// It should be between 2 and 256, and should be a power of 2
	QuadGridMesh(const int dimension) : m_mesh(dimension / 2) {
		assert(2 <= dimension && dimension <= 256);
	}

private:
	GridMesh m_mesh;
};
