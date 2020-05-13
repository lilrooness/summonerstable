#version 330 core


in vec2 texCoord;
in vec2 position;
in float on;

out vec4 LFragment;

uniform sampler2D sampler;

void main() {
//	vec4 tint;
//	if(on == 1.0f) {
//		tint = vec4(1.5f, 1.5f, 1.0f, 1.0f);
//	} else {
//		tint = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	}
//	LFragment = texture(sampler, texCoord) * tint;
	LFragment = texture(sampler, texCoord);
}

