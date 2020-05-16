#pragma once
//TODO: Tint is just a float, this should be a vec4 color
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GL/GLU.h>

#include "shader_reader.h"

struct SpriteMaterial {
	unsigned int id;
	GLuint BufferHandle_VAO;
	GLuint BufferHandle_vertexAndUVData;
	GLuint BufferHandle_indexData;

	//Vertex Shader Attributes
	GLuint BufferHandle_vertexOffsetData;
	GLuint BufferHandle_textureOffsetData;
	
	GLuint BufferHandleInstanced_tintData;
	GLuint BufferHandleInstanced_scaleData;

	GLuint shaderProgramID;
};

const GLuint     VERTEX_POS_ATTRIB_LOCATION = 3;
const GLuint     TEXTURE_UV_ATTRIB_LOCATION = 4;
const GLuint           TINT_ATTRIB_LOCATION = 5;
const GLuint          SCALE_ATTRIB_LOCATION = 6;
const GLuint  VERTEX_OFFSET_ATTRIB_LOCATION = 7;
const GLuint TEXTURE_OFFSET_ATTRIB_LOCATION = 8;

GLenum initSpriteMaterial(
	GLfloat* vertexAndUVData,
	int vertexAndUVDataSize,
	std::vector<GLfloat> instancedTintData,
	std::vector<GLfloat> instancedScaleData,
	std::vector<GLfloat> vertexOffsetData,
	std::vector<GLfloat> textureOffsetData,
	SpriteMaterial *spriteMaterial_out
) {

	glGenVertexArrays(1, &spriteMaterial_out->BufferHandle_VAO);
	glBindVertexArray(spriteMaterial_out->BufferHandle_VAO);

	//TEXTURE OFFSET DATA
	glGenBuffers(1, &spriteMaterial_out->BufferHandle_textureOffsetData);
	glBindBuffer(GL_ARRAY_BUFFER, spriteMaterial_out->BufferHandle_textureOffsetData);
	glBufferData(GL_ARRAY_BUFFER, textureOffsetData.size() * sizeof(GLfloat), textureOffsetData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(TEXTURE_OFFSET_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(TEXTURE_OFFSET_ATTRIB_LOCATION);
	glVertexAttribDivisor(TEXTURE_OFFSET_ATTRIB_LOCATION, 1);

	//VERTEX OFFSET DATA
	glGenBuffers(1, &spriteMaterial_out->BufferHandle_vertexOffsetData);
	glBindBuffer(GL_ARRAY_BUFFER, spriteMaterial_out->BufferHandle_vertexOffsetData);
	glBufferData(GL_ARRAY_BUFFER, vertexOffsetData.size() * sizeof(GLfloat), vertexOffsetData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_OFFSET_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(VERTEX_OFFSET_ATTRIB_LOCATION);
	glVertexAttribDivisor(VERTEX_OFFSET_ATTRIB_LOCATION, 1);

	//VERTEX & UV DATA
	glGenBuffers(1, &spriteMaterial_out->BufferHandle_vertexAndUVData);
	glBindBuffer(GL_ARRAY_BUFFER, spriteMaterial_out->BufferHandle_vertexAndUVData);
	glBufferData(GL_ARRAY_BUFFER, vertexAndUVDataSize * sizeof(GLfloat), vertexAndUVData, GL_STATIC_DRAW);
	
	glVertexAttribPointer(VERTEX_POS_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(VERTEX_POS_ATTRIB_LOCATION);

	glVertexAttribPointer(TEXTURE_UV_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)(sizeof(float) * 12));
	glEnableVertexAttribArray(TEXTURE_UV_ATTRIB_LOCATION);

	//INSTANCED SCALE DATA
	glGenBuffers(1, &spriteMaterial_out->BufferHandleInstanced_scaleData);
	glBindBuffer(GL_ARRAY_BUFFER, spriteMaterial_out->BufferHandleInstanced_scaleData);
	glBufferData(GL_ARRAY_BUFFER, instancedScaleData.size() * sizeof(GLfloat), instancedScaleData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(SCALE_ATTRIB_LOCATION, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(SCALE_ATTRIB_LOCATION);
	glVertexAttribDivisor(SCALE_ATTRIB_LOCATION, 1);

	//INSTANCED TINT DATA
	glGenBuffers(1, &spriteMaterial_out->BufferHandleInstanced_tintData);
	glBindBuffer(GL_ARRAY_BUFFER, spriteMaterial_out->BufferHandleInstanced_tintData);
	glBufferData(GL_ARRAY_BUFFER, instancedTintData.size() * sizeof(GLfloat), instancedTintData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(TINT_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(TINT_ATTRIB_LOCATION);
	glVertexAttribDivisor(TINT_ATTRIB_LOCATION, 1);


	//INDEX DATA
	GLuint quadIndexData[] = { 0, 1, 2, 0, 2, 3 };
	glGenBuffers(1, &spriteMaterial_out->BufferHandle_indexData);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteMaterial_out->BufferHandle_indexData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), quadIndexData, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return glGetError();
}

template<typename T>
void refreshBuffer(GLenum bufferType, GLuint bufferHandle, std::vector<T> bufferData, GLenum usage) {
	glBindBuffer(bufferType, bufferHandle);
	glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);
}