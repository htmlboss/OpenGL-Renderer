#pragma once

#include "QuadGridMesh.h"

#include <glm/vec3.hpp>
#include <glad/glad.h>

#include <memory>

/*
class QuadTree {
	
public:
	
	auto GetNodeDimension() const noexcept {
		return m_nodeDimension;
	}

private:
	struct Node {
		Node(GLshort x, GLshort z, GLubyte level, GLubyte dimension, bool root = false);

		// Helper to create the quadtree in four threads
		static void Init(GLshort x, GLshort z, GLubyte level,
			GLubyte dimension, std::unique_ptr<Node>* node);

		auto collidesWithSphere(const glm::vec3& center, const float radius) const {
			return bbox.collidesWithSphere(center, radius);
		}

		void countMinMaxOfArea(const HeightMapInterface& hmap, double* min, double* max, const bool root = false);

		// Helper to run countMinMaxOfArea in thread
		static void CountMinMaxOfArea(Node* node, const HeightMapInterface& hmap, double* min, double* max) {
			node->countMinMaxOfArea(hmap, min, max);
		}

		void selectNodes(const glm::vec3& cam_pos, const Frustum& frustum, QuadGridMesh& grid_mesh, int node_dimension);

		GLshort x, z;
		BoundingBox bbox;
		GLushort size;
		GLubyte level;
		std::unique_ptr<Node> tl, tr, bl, br;
	};

	QuadGridMesh m_mesh;
	GLubyte m_nodeDimension;
	Node m_root;
};
*/
