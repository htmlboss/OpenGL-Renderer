#include "Skybox.h"

#include <iostream>

// Easier to use TS library than hard-code texture files
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

/***********************************************************************************/
Skybox::Skybox(const std::string& TextureDirectory) {
	
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyboxVertices), &m_skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	// Cleanup
	glBindVertexArray(0);
	
	// Iterate through given directory and find files
	int i = 0; // Index for the loop
	for (auto& it : fs::directory_iterator(TextureDirectory)) {
		m_faces.at(i) = it.path().generic_string(); // put texture paths into std::array for loading
		++i;
	}

	// Generate textures
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	
	unsigned char* image;
	int x, y;
	int n = 3; //Number of components to load (RGBA)
	i = 0; // Index for the loop
	for (auto& face : m_faces) {
		image = Texture::LoadSTBImage(face.c_str(), &x, &y, &n, 3); // 3 = STB_rgb
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		free(image); // Cleanup
		++i;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

/***********************************************************************************/
Skybox::~Skybox() {
}

/***********************************************************************************/
void Skybox::Draw(Shader& shader, const glm::mat4& CameraMatrix, const glm::mat4& ProjectionMat) {
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	shader.Use();

	// Transformations
	glm::mat4 view = glm::mat4(glm::mat3(CameraMatrix));	// Remove any translation component of the view matrix	
	glUniformMatrix4fv(shader.GetUniformLoc("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader.GetUniformLoc("projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMat));
	
	// Skybox Cube
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shader.GetUniformLoc("skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	// Cleanup
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}
