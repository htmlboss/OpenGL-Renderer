#pragma once

#include <cstddef>

//http://victorbush.com/2015/01/tessellated-terrain/

/***********************************************************************************/
// Forward Declarations
struct TerrainNode;

/***********************************************************************************/
class TerrainRenderer {

public:
	TerrainRenderer() = default;

	void Init();
	void Render();
	void Shutdown();

private:
	void clearTerrainTree();
	// Calculate the tessellation scale factor for a node depending on the neighboring patches
	void calcTesselationScale(TerrainNode* node);
	// Search for a node in the tree given xz coordinate
	TerrainNode* findNode(TerrainNode* node, const float x, const float z);

	TerrainNode* m_terrainTree = nullptr;
	TerrainNode* m_terrainTreeTail = nullptr;
	
	const std::size_t m_MaxTerrainNodes = 500;
	std::size_t m_numTerrainNodes = 0;
};