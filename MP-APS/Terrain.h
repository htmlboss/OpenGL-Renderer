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
	// Generate vertices
	std::vector<Vertex> generateVertices() const;
	// Generate indices
	std::vector<GLuint> calculateIndices() const;
	// Load textures
	std::vector<GLTexture> loadTextures() const;
	// Generate height
	float generateHeight(const int x, int z) const;

	ModelPtr m_terrainModel;
	mutable std::shared_ptr<Texture> m_heightmap;

	mutable std::size_t VERTEX_COUNT;
	const std::size_t SIZE = 800, MAX_HEIGHT = 40, MAX_PIXEL_COLOR = 256*256*256;
	const int MIN_HEIGHT = -40;
	const std::size_t m_x, m_z; // since +Y is vertical
};

