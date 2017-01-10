#pragma once
#include <array>

// Contains predefined vertices and TexCoords for a Screen Quad in NDC.
struct ScreenQuad {
	const std::array<float, 24> Vertices = {
		// Positions   TexCoords

		// Triangle 1
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

		// Triangle 2
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
		1.0f,  1.0f,  1.0f, 1.0f // Top-right
	};
};