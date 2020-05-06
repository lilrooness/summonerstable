#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

struct InstancedSpriteShaderAttribute_Float {
	int id;
	const char* name;
	GLuint location;
	GLfloat *instancedData;
	int instancedDataSize;
	int dim;
	GLuint bufferHandle;
};

struct InstancedSpriteMeshMaterial2D {
	GLuint vao;
	GLuint vertexDataVBO;
	GLuint textureDataVBO;
	GLuint textureOffsetVBO_instanced;
	GLuint positionOffsetVBO;
	GLuint textureOffsetVBO;
	GLuint indexDataBuffer;
	std::vector<InstancedSpriteShaderAttribute_Float> shaderAttributes;
};

GLenum initInstancedSpriteMeshMaterial_2D(
	
	GLint gVertexPos2DLocation, GLint gTextureTranslationLocation, GLint gTextureUVLocation, GLint gVertexTranslationLocation,
	GLfloat* modelVertexAndUVData, int modelVertxeAndUVDataSize, 
	GLfloat* instancedTextureOffsets, int textureOffsetsSize, 
	GLfloat* positionOffsets, int positionOffsetsSize,
	std::vector<InstancedSpriteShaderAttribute_Float> shaderAttributes,
	InstancedSpriteMeshMaterial2D* out_meshMaterial) {

	glGenVertexArrays(1, &out_meshMaterial->vao);
	glBindVertexArray(out_meshMaterial->vao);

	glGenBuffers(1, &out_meshMaterial->textureOffsetVBO_instanced);
	glBindBuffer(GL_ARRAY_BUFFER, out_meshMaterial->textureOffsetVBO_instanced);
	glBufferData(GL_ARRAY_BUFFER, textureOffsetsSize * sizeof(GLfloat), instancedTextureOffsets, GL_STATIC_DRAW);

	glVertexAttribPointer(gTextureTranslationLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(gTextureTranslationLocation);
	glVertexAttribDivisor(gTextureTranslationLocation, 1);

	glGenBuffers(1, &out_meshMaterial->positionOffsetVBO);
	glBindBuffer(GL_ARRAY_BUFFER, out_meshMaterial->positionOffsetVBO);
	glBufferData(GL_ARRAY_BUFFER, positionOffsetsSize * sizeof(GLfloat), positionOffsets, GL_STATIC_DRAW);

	glVertexAttribPointer(gVertexTranslationLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(gVertexTranslationLocation);
	glVertexAttribDivisor(gVertexTranslationLocation, 1);

	glGenBuffers(1, &out_meshMaterial->vertexDataVBO);
	glBindBuffer(GL_ARRAY_BUFFER, out_meshMaterial->vertexDataVBO);
	glBufferData(GL_ARRAY_BUFFER, modelVertxeAndUVDataSize * sizeof(GLfloat), modelVertexAndUVData, GL_STATIC_DRAW);

	glVertexAttribPointer(gVertexPos2DLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(gVertexPos2DLocation);
	glVertexAttribPointer(gTextureUVLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)(sizeof(float) * 12));
	glEnableVertexAttribArray(gTextureUVLocation);

	for (InstancedSpriteShaderAttribute_Float& attrib : shaderAttributes) {
		glGenBuffers(1, &attrib.bufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, attrib.bufferHandle);
		glBufferData(GL_ARRAY_BUFFER, attrib.instancedDataSize * sizeof(GLfloat), attrib.instancedData, GL_STATIC_DRAW);

		glVertexAttribPointer(attrib.location, attrib.dim, GL_FLOAT, GL_FALSE, attrib.dim * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(attrib.location);
		glVertexAttribDivisor(attrib.location, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	out_meshMaterial->shaderAttributes = shaderAttributes;

	GLuint quadIndexData[] = { 0, 1, 2, 0, 2, 3 };

	glGenBuffers(1, &out_meshMaterial->indexDataBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_meshMaterial->indexDataBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), quadIndexData, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	return glGetError();
}