#version 440 core

in float VertexDepth;

out vec4 FragColor;

void main() {             
	const float depth = VertexDepth;

	const float dx = dFdx(depth);
	const float dy = dFdy(depth);
	const float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

	FragColor = vec4(depth, moment2, 0.0, 1.0);
}