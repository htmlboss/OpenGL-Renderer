#version 440 compatibility

#ifndef SMAA_PIXEL_SIZE
	#define SMAA_PIXEL_SIZE vec2(1.0 / 1280.0, 1.0 / 720.0)
#endif

#define SMAA_PRESET_ULTRA 1
#define SMAA_GLSL_4 1
#define SMAA_ONLY_COMPILE_PS 1

#include "Data/Shaders/smaa/SMAA.h"

uniform sampler2D albedo_tex;
uniform sampler2D blend_tex;

in vec2 texcoord;
in vec4 offset[2];
in vec4 dummy2;

void main() {
  gl_FragColor = SMAANeighborhoodBlendingPS(texcoord, offset, albedo_tex, blend_tex);
}