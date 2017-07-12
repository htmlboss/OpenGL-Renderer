#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
	using vec2 = glm::vec2;
	using vec3 = glm::vec3;

	Vertex() = default;

	Vertex(const vec3& position, const vec2& texcoords) noexcept : Position(position),
	                                                               TexCoords(texcoords) {
	}

	Vertex(const vec3& position, const vec2& texcoords, const vec3& normal) noexcept : Position(position),
	                                                                                   TexCoords(texcoords),
	                                                                                   Normal(normal) {
	}

	Vertex(const vec3& position, const vec2& texcoords, const vec3& normal, const vec3& tangent, const vec3& bitangent) noexcept : Position(position),
	                                                                                                                               TexCoords(texcoords),
	                                                                                                                               Normal(normal),
	                                                                                                                               Tangent(tangent),
	                                                                                                                               Bitangent(bitangent) {
	}

	vec3 Position;
	vec2 TexCoords;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
};
