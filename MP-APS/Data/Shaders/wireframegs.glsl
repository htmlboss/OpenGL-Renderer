#version 440 core

layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
	vec2 TexCoords;
	vec3 FragPos;
	mat3 TBN;
	vec4 FragPosLightSpace;
} inData[];

out FragData {
	vec2 TexCoords;
	vec3 FragPos;
	mat3 TBN;
	vec4 FragPosLightSpace;
	// Noperspective so the interpolation is in screen-space
	noperspective vec3 wireframeDist;
} outData;

void main() {

	for (int i = 0; i < 3; ++i) {
		gl_Position = gl_in[i].gl_Position;

		// Pass-through our vertex data
		outData.TexCoords = inData[i].TexCoords;
		outData.FragPos = inData[i].FragPos;
		outData.TBN = inData[i].TBN;
		outData.FragPosLightSpace = inData[i].FragPosLightSpace;

		// The attribute will be interpolated, so
        // all you have to do is set the ith dimension to 1.0 to get barycentric coordinates
        // specific to this triangle. The frag shader will interpolate and then you can just use
        // a threshold in the frag shader to figure out if you're close to an edge
        outData.wireframeDist = vec3(0.0);
        outData.wireframeDist[i] = 1.0;

		EmitVertex();
	}
}