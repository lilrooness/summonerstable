#version 330 core

layout (location = 7) in vec3 APos;
layout (location = 8) in vec2 AUVOffset;

layout (location = 3) in vec3 LVertexPos2D;
layout (location = 4) in vec2 ATexCoord;
layout (location = 5) in vec3 tintValue;
layout (location = 6) in float scaleValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 tint;

void main() {
	texCoord = ATexCoord + AUVOffset;
	tint = tintValue;
	vec4 world_position =  vec4(vec3(scaleValue), 1.0f) * model * vec4(LVertexPos2D, 1 ) + vec4(APos, 0);
	gl_Position = projection * view * world_position;
}