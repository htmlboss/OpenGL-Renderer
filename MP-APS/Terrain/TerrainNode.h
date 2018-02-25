#pragma once

#include <glm/vec3.hpp>

#define TERRAIN_SUBDIV_CUTTOF 100

// http://victorbush.com/2015/01/tessellated-terrain/

struct TerrainNode {

	glm::vec3 Origin;
	float Width, Height;
	unsigned int Type; // Child # relative to parent

	// Tesselation scaling
	float TScaleNegX, TScalePosX;
	float TScaleNegZ, TScalePosZ;
	
	TerrainNode* Parent;
	TerrainNode* Child1;
	TerrainNode* Child2;
	TerrainNode* Child3;
	TerrainNode* Child4;

	TerrainNode* North;
	TerrainNode* South;
	TerrainNode* East;
	TerrainNode* West;
};
