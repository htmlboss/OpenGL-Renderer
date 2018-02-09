#pragma once

struct TerrainNode {
	
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
