#pragma once

#include <GL/glew.h>
#include <array>

class GLShaderProgram;

class Light {
	typedef std::array<float, 3> Vec3;
public:
	
	enum LightType {
		POINTLIGHT,
		SPOTLIGHT,
		DIRLIGHT
	};

	Light(const Vec3 Position, const Vec3 Color, const LightType lightType);
	~Light();

	//void Draw(GLShaderProgram& shader);

private:
	Vec3 m_position;
	Vec3 m_color;

	GLuint m_vao, m_vbo;
};