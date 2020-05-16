#version 330 core

in vec2 texCoord;
in vec3 tint;

out vec4 LFragment;

uniform sampler2D sampler;

void main() {
	LFragment = texture(sampler, texCoord) * vec4(tint, 1.0f);
}