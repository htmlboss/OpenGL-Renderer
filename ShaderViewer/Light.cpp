#include "Light.h"
#include "CubeData.h"

/***********************************************************************************/
Light::Light(const glm::vec3& Position, const glm::vec3& Color, const LightType lightType) :
			m_position(Position), 
			m_color(Color) {

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid*>(0));

	glBindVertexArray(0);
}

/***********************************************************************************/
Light::~Light() {
}

/***********************************************************************************/
void Light::Draw(Shader& shader) {
	
	shader.Bind();

	shader.SetUniform("lightColor", m_color);
	glm::mat4 model;
	model = glm::translate(model, m_position);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	shader.SetUniform("model", model);
	
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
