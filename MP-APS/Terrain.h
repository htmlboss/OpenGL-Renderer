#pragma once

#include "Model.h"

#include <memory>

class Terrain {
public:
	Terrain(const std::size_t gridX, const std::size_t gridZ);
	~Terrain() = default;

	void Draw(GLShaderProgram* shader) const {
		shader->Bind();
		shader->SetUniform("modelMatrix", m_terrainModel->GetModelMatrix());
		shader->SetUniform("lightPos", glm::vec3(0.0f, 2.0f, 5.0f));
		shader->SetUniform("lightColor", glm::vec3(1.0f));
		shader->SetUniformf("shineDamper", 1.0f);
		shader->SetUniformf("reflectivity", 0.0f);

		m_terrainModel->Draw(shader);
	}

private:
	Mesh generateTerrain() const;
	std::vector<Vertex> generateVertices() const;
	// Generate indices
	std::vector<GLuint> calculateIndices() const;

	ModelPtr m_terrainModel;
	const std::size_t SIZE = 800, VERTEX_COUNT = 128; // Terrain size and side vertex count
	const std::size_t m_x, m_z; // since +Y is vertical
};

