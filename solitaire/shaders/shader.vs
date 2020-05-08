#version 140

in vec3 APos;
in vec2 ATexCoord;
in vec2 AUVOffset;
in vec3 LVertexPos2D;
in float scaleValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main() {
	texCoord = ATexCoord + AUVOffset;
	vec4 world_position =  vec4(vec3(scaleValue), 1.0f) * model * vec4(LVertexPos2D, 1 ) + vec4(APos, 0);
	gl_Position = projection * view * world_position;
}