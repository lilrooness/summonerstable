#version 330 core


in vec2 texCoord;

out vec4 LFragment;

uniform sampler2D sampler;

void main() {
	LFragment = texture(sampler, texCoord);
}