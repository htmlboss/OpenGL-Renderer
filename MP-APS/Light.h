#pragma once

#include "glad/glad.h"
#include <array>

struct Light {
	using Vec3 = std::array<float, 3>;
	
	enum LightType {
		POINTLIGHT,
		SPOTLIGHT,
		DIRLIGHT
	};

	Light(const Vec3 Position, const Vec3 Color, const LightType lightType) : m_position(Position), m_color(Color), m_lightType(lightType) {}

	Vec3 m_position, m_color;
	const LightType m_lightType;

	static GLuint m_vao, m_vbo;
};