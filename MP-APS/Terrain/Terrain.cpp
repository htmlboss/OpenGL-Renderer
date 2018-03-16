#include "Terrain.h"
#include "TerrainNode.h"

#include "../Graphics/GLShader.h"
#include "../ResourceManager.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cstring>
#include <cmath>

/***********************************************************************************/
void Terrain::Init(const std::string_view heightmapPath) {

	m_terrainTree = new TerrainNode[m_MaxTerrainNodes];
	clearTerrainTree();

	/*
	m_program.Init("Terrain Shader", {	GLShader("", GL_VERTEX_SHADER),
										GLShader("", GL_TESS_CONTROL_SHADER), 
										GLShader("", GL_TESS_EVALUATION_SHADER), 
										GLShader("", GL_GEOMETRY_SHADER), 
										GLShader("", GL_FRAGMENT_SHADER)	}
	);
	*/

	m_program.Bind();
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	const auto heightmap = ResourceManager::GetInstance().LoadTexture("", true, true);
	glBindTexture(GL_TEXTURE_2D, heightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//m_program.SetUniformi();

	const float quadData[] {
		// Vert 1
		-1.0f, 0.0f, -1.0f, 1.0,	// Position
		0.18f, 0.91f, 0.46f, 1.0,	// Color
		0.0f, 1.0f, 0.0f, 0.0f,		// Normal
		0.0f, 2.0f,					// Tex coord (u,v)

		// Vert 2
		1.0f, 0.0f, -1.0f, 1.0,		// Position
		0.18f, 0.91f, 0.46f, 1.0,	// Color
		0.0f, 1.0f, 0.0f, 0.0f,		// Normal
		2.0f, 2.0f,					// Tex coord (u,v)

		// Vert 3
		1.0f, 0.0f, 1.0f, 1.0,		// Position
		0.18f, 0.91f, 0.46f, 1.0,	// Color
		0.0f, 1.0f, 0.0f, 0.0f,		// Normal
		2.0f, 0.0f,					// Tex coord (u,v)

		// Vert 4
		-1.0f, 0.0f, 1.0f, 1.0,		// Position
		0.18f, 0.91f, 0.46f, 1.0,	// Color
		0.0f, 1.0f, 0.0f, 0.0f,		// Normal
		0.0f, 0.0f,					// Tex coord (u,v)
	};

	const GLuint quadIndicies[] { 0, 3, 2, 0, 2, 1 };
	const GLuint quadPatchInd[] { 0, 1, 2, 3 };

	m_vao.Init();
	m_vao.Bind();

	m_vao.AttachBuffer(GLVertexArray::BufferType::ARRAY, 14 * 4 * sizeof(float), GLVertexArray::DrawMode::STATIC, quadData);
	m_vao.EnableAttribute(0, 4, 14 * sizeof(float), nullptr);

	m_vao.AttachBuffer(GLVertexArray::BufferType::ELEMENT, 4 * sizeof(float), GLVertexArray::DrawMode::STATIC, quadPatchInd);
}

/***********************************************************************************/
void Terrain::Update(const float x, const float y, const float z, const float width, const float height, const glm::vec3& camPos) {
	
	clearTerrainTree();

	m_terrainTree->Type = 0; // Root node
	m_terrainTree->Origin = {x, y, z};
	m_terrainTree->Width = width;
	m_terrainTree->Height = height;
	m_terrainTree->TScaleNegX = 1.0f;
	m_terrainTree->TScaleNegZ = 1.0f;
	m_terrainTree->TScalePosX = 1.0f;
	m_terrainTree->TScalePosZ = 1.0f;
	m_terrainTree->Parent = nullptr;
	m_terrainTree->North = nullptr;
	m_terrainTree->South = nullptr;
	m_terrainTree->East = nullptr;
	m_terrainTree->West = nullptr;

	// Recursively subdivide the terrain
	divideNode(m_terrainTree, camPos);
}

/***********************************************************************************/
void Terrain::Render(const glm::mat4x4& viewMatrix) {
	m_renderDepth = 0;
	m_program.Bind();

	renderRecursive(m_terrainTree, viewMatrix);
}

/***********************************************************************************/
void Terrain::Shutdown() {
	delete[] m_terrainTree;

	m_terrainTree = nullptr;
	m_terrainTreeTail = nullptr;
}

/***********************************************************************************/
void Terrain::clearTerrainTree() {
	m_terrainTreeTail = m_terrainTree;
	std::memset(m_terrainTree, 0, m_MaxTerrainNodes * sizeof(TerrainNode));
	m_numTerrainNodes = 0;
}

/***********************************************************************************/
void Terrain::renderRecursive(TerrainNode* node, const glm::mat4x4& viewMatrix) {
	
	// If all children are null, render this node
	if (!node->Child1 && !node->Child2 && !node->Child3 && !node->Child4) {
		renderNode(node, viewMatrix);
		++m_renderDepth;
		return;
	}

	// Note: we're checking if the child exists. Theoretically, with our algorithm,
	// either all the children are null or all the children are not null.
	// There shouldn't be any other cases, but we check here for safety.
	if (node->Child1) {
		renderRecursive(node->Child1, viewMatrix);
	}
	if (node->Child2) {
		renderRecursive(node->Child2, viewMatrix);
	}
	if (node->Child3) {
		renderRecursive(node->Child3, viewMatrix);
	}
	if (node->Child4) {
		renderRecursive(node->Child4, viewMatrix);
	}
}

/***********************************************************************************/
void Terrain::renderNode(TerrainNode* node, const glm::mat4x4& viewMatrix) {
	
	// Calculate the tess scale factor
	calcTesselationScale(node);
	
	// https://bitbucket.org/victorbush/ufl.cap5705.terrain/src/93c5ab3824a5a66d87d1bb6dcc9ed9aee7a16357/src_non_uniform/terrain.c?at=master&fileviewer=file-view-default#terrain.c-231
	
	glm::mat4x4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, node->Origin);

	const auto modelViewMatrix = viewMatrix * modelMatrix;

	m_program.SetUniform("mMatrix", modelMatrix);
	m_program.SetUniform("mvMatrix", modelViewMatrix);

	glm::mat3x3 normalMatrix(modelViewMatrix);
	normalMatrix = glm::transpose(normalMatrix);
	normalMatrix = glm::inverse(normalMatrix);

	m_program.SetUniform("nMatrix", normalMatrix);
	
	m_program.SetUniformf("tileScale", 0.5f * node->Width);
	
	m_program.SetUniformf("tscale_negx", node->TScaleNegX);
	m_program.SetUniformf("tscale_negz", node->TScaleNegZ);
	m_program.SetUniformf("tscale_posx", node->TScalePosX);
	m_program.SetUniformf("tscale_posz", node->TScalePosZ);

	glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, nullptr);
}

/***********************************************************************************/
void Terrain::calcTesselationScale(TerrainNode* node) {

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
TerrainNode* Terrain::findNode(TerrainNode* node, const float x, const float z) {
	
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

/***********************************************************************************/
TerrainNode* Terrain::createNode(TerrainNode* parent, const int type, const float x, const float y, const float z, const float width, const float height) {
	// If there is room in the quadtree memory block
	if (m_numTerrainNodes >= m_MaxTerrainNodes) {
		return nullptr;
	}
	++m_numTerrainNodes;

	++m_terrainTreeTail;
	m_terrainTreeTail->Type = type;
	m_terrainTreeTail->Origin = { x, y, z };
	m_terrainTreeTail->Width = width;
	m_terrainTreeTail->Height = height;
	m_terrainTreeTail->TScaleNegX = 1.0f;
	m_terrainTreeTail->TScaleNegZ = 1.0f;
	m_terrainTreeTail->TScalePosX = 1.0f;
	m_terrainTreeTail->TScalePosZ = 1.0f;
	m_terrainTreeTail->Parent = parent;
	m_terrainTreeTail->North = nullptr;
	m_terrainTreeTail->South = nullptr;
	m_terrainTreeTail->East = nullptr;
	m_terrainTreeTail->West = nullptr;
	
	return m_terrainTreeTail;
}

/***********************************************************************************/
void Terrain::divideNode(TerrainNode* node, const glm::vec3& camPos) {
	
	// Subdivide
	const float w_new = 0.5f * node->Width;
	const float h_new = 0.5f * node->Height;

	// Create the child nodes
	node->Child1 = createNode(node, 1, node->Origin[0] - 0.5f * w_new, node->Origin[1], node->Origin[2] - 0.5f * h_new, w_new, h_new);
	node->Child2 = createNode(node, 2, node->Origin[0] + 0.5f * w_new, node->Origin[1], node->Origin[2] - 0.5f * h_new, w_new, h_new);
	node->Child3 = createNode(node, 3, node->Origin[0] + 0.5f * w_new, node->Origin[1], node->Origin[2] + 0.5f * h_new, w_new, h_new);
	node->Child4 = createNode(node, 4, node->Origin[0] - 0.5f * w_new, node->Origin[1], node->Origin[2] + 0.5f * h_new, w_new, h_new);

	// Assign neighbors
	switch (node->Type) {
	case 1:
		node->East = node->Parent->Child2;
		node->North = node->Parent->Child4;
		break;
	case 2:
		node->West = node->Parent->Child1;
		node->North = node->Parent->Child3;
		break;
	case 3:
		node->South = node->Parent->Child2;
		node->West = node->Parent->Child4;
		break;
	case 4:
		node->South = node->Parent->Child1;
		node->East = node->Parent->Child3;
		break;
	}

	// Check if each of these four child nodes will be subdivided.
	const auto div1 = shouldDivideNode(node->Child1, camPos);
	const auto div2 = shouldDivideNode(node->Child2, camPos);
	const auto div3 = shouldDivideNode(node->Child3, camPos);
	const auto div4 = shouldDivideNode(node->Child4, camPos);
	
	if (div1) {
		divideNode(node->Child1, camPos);
	}
	if (div2) {
		divideNode(node->Child2, camPos);
	}
	if (div3) {
		divideNode(node->Child3, camPos);
	}
	if (div4) {
		divideNode(node->Child4, camPos);
	}
}

/***********************************************************************************/
bool Terrain::shouldDivideNode(TerrainNode* node, const glm::vec3& camPos) const {

	// Distance from current origin to camera
	const auto d = std::abs(std::sqrt(std::pow(camPos[0] - node->Origin[0], 2.0f) + pow(camPos[2] - node->Origin[2], 2.0f)));

	// Check base case:
	// Distance to camera is greater than twice the length of the diagonal
	// from current origin to corner of current square.
	// OR
	// Max recursion level has been hit
	if (d > 2.5f * std::sqrt(std::pow(0.5f * node->Width, 2.0f) + std::pow(0.5f * node->Height, 2.0f)) || node->Width < TERRAIN_SUBDIV_CUTTOF) {
		return false;
	}

	return true;
}
