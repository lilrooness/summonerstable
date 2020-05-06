#version 140

in vec3 APos;
in vec2 ATexCoord;
in vec2 AUVOffset;
in vec3 LVertexPos2D;
in float candleOn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec2 position;
out float on;

void main() {
	position = LVertexPos2D.xy;
	texCoord = ATexCoord + AUVOffset;
	vec4 world_position = model * vec4(LVertexPos2D, 1 ) + vec4(APos, 0);
	on = candleOn;
	gl_Position = projection * view * world_position;
}