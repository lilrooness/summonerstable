#version 140

in vec2 texCoord;

out vec4 LFragment;

uniform sampler2D sampler;

void main() {
	LFragment = texture(sampler, texCoord);
}