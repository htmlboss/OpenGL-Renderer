#version 440 core

uniform float near;
uniform float far;

out vec4 FragColor;

// Need to linearize the depth because we are using the projection
float LinearizeDepth(const float depth) {
	const float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    
    const float depth = LinearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(depth), 1.0);
}