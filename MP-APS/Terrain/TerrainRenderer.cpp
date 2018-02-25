#include "TerrainRenderer.h"
#include "TerrainNode.h"

#include <cstring>

/***********************************************************************************/
void TerrainRenderer::Init() {

	m_terrainTree = new TerrainNode[m_MaxTerrainNodes];
	clearTerrainTree();
}

/***********************************************************************************/
void TerrainRenderer::Render() {
}

/***********************************************************************************/
void TerrainRenderer::Shutdown() {
	delete[] m_terrainTree;

	m_terrainTree = nullptr;
	m_terrainTreeTail = nullptr;
}

/***********************************************************************************/
void TerrainRenderer::clearTerrainTree() {
	m_terrainTreeTail = m_terrainTree;
	std::memset(m_terrainTree, 0, m_MaxTerrainNodes * sizeof(TerrainNode));
	m_numTerrainNodes = 0;
}

/***********************************************************************************/
void TerrainRenderer::calcTesselationScale(TerrainNode* node) {

	// Positive Z (north)
	TerrainNode* temp = findNode(m_terrainTree, node->Origin[0], node->Origin[2] + 1 + node->Width / 2.0f);
	if (temp->Width > node->Width) {
		node->TScalePosZ = 2.0f;
	}

	// Positive X (east)
	temp = findNode(m_terrainTree, node->Origin[0] + 1 + node->Width / 2.0f, node->Origin[2]);
	if (temp->Width > node->Width) {
		node->TScalePosX = 2.0f;
	}

	// Negative Z (south)
	temp = findNode(m_terrainTree, node->Origin[0], node->Origin[2] - 1 - node->Width / 2.0f);
	if (temp->Width > node->Width) {
		node->TScaleNegZ = 2.0f;
	}

	// Negative X (west)
	temp = findNode(m_terrainTree, node->Origin[0] - 1 - node->Width / 2.0f, node->Origin[2]);
	if (temp->Width > node->Width) {
		node->TScaleNegX = 2.0f;
	}
}

/***********************************************************************************/
TerrainNode* TerrainRenderer::findNode(TerrainNode* node, const float x, const float z) {
	
	if (node->Origin[0] == x && node->Origin[2] == z) {
		return node;
	}

	if (node->Child1 == nullptr && node->Child2 == nullptr && node->Child3 == nullptr && node->Child4 == nullptr) {
		return node;
	}

	if (node->Origin[0] >= x && node->Origin[2] >= z && node->Child1) {
		return findNode(node->Child1, x, z);
	}
	
	if (node->Origin[0] <= x && node->Origin[2] >= z && node->Child2) {
		return findNode(node->Child2, x, z);
	}
	
	if (node->Origin[0] <= x && node->Origin[2] <= z && node->Child3) {
		return findNode(node->Child3, x, z);
	}
	
	if (node->Origin[0] >= x && node->Origin[2] <= z && node->Child4) {
		return findNode(node->Child4, x, z);
	}

	return node;
}
