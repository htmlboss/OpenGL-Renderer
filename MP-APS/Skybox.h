#pragma once
#include "GL/GLShaderProgram.h"


class Skybox {
public:
	explicit Skybox(const std::string_view hdrPath, const std::size_t resolution = 512);

	void Draw();

private:
	unsigned int m_vao, m_vbo, m_envMap, m_envMapFBO;
	
	GLShaderProgram m_skyboxShader;

};
