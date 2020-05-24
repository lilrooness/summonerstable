#pragma once

// TODO: SpriteAnimation - replace spriteIndex with an IndexReference

#include "sprite_material.h"
#include "reference.h"
#include "animation.h"

struct SpriteAnimation;
struct SpriteClass;
struct Sprite;

IndexReference queueScaleAnimation(SpriteClass& spriteClass, int spriteIndex, float from, float to, float duration, float startedAt);
void cancelScaleAnimation(SpriteClass& spriteClass, IndexReference indexReference, const Sprite& sprite);
template<typename T>
void resolveScaleAnimations(SpriteClass& spriteClass, std::vector<T> spriteObjects, float gameTime);

struct Sprite {
	int BufferIndex_vertexOffsetData;
	int BufferIndex_textureOffsetData;
	int BufferIndex_tintValueData;
	int BufferIndex_scaleValueData;
	IndexReference scaleAnimationReference;
};

struct SpriteAnimation {
	FloatAnimation flaotAnimation;
	int spriteIndex;
	int	spriteGeneration;
};

struct SpriteClass {
	SpriteMaterial material;

	std::vector<GLfloat> Buffer_vertexOffsetData;
	std::vector<GLfloat> Buffer_textureOffsetData;
	std::vector<GLfloat> Buffer_tintValueData;
	std::vector<GLfloat> Buffer_scaleValueData;

	bool BufferRefreshFlag_vertexOffsetData;
	bool BufferRefreshFlag_textureOffsetData;
	bool BufferRefreshFlag_tintValueData;
	bool BufferRefreshFlag_scaleValueData;

	std::vector<SpriteAnimation> scaleAnimations;

	float defaultScale{ 1.0f };
};

bool validScaleAnimationReference(SpriteClass& spriteClass, IndexReference& reference) {
	return reference.index < spriteClass.scaleAnimations.size() 
		&& spriteClass.scaleAnimations[reference.index].spriteGeneration == reference.generation 
		&& !spriteClass.scaleAnimations[reference.index].flaotAnimation.done;
}

template<typename T>
void resolveScaleAnimations(SpriteClass& spriteClass, std::vector<T> spriteObjects, float gameTime) {
	for (int i = 0; i < spriteClass.scaleAnimations.size(); i++) {
		if (!spriteClass.scaleAnimations[i].flaotAnimation.done) {
			int spriteIndex = spriteClass.scaleAnimations[i].spriteIndex;
			int scaleValueBufferIndex = spriteObjects[spriteIndex].sprite.BufferIndex_scaleValueData;
			spriteClass.Buffer_scaleValueData[scaleValueBufferIndex] = getCurrentAnimationValue(spriteClass.scaleAnimations[i].flaotAnimation, gameTime);
			spriteClass.BufferRefreshFlag_scaleValueData = true;
		}
	}
}

void cancelScaleAnimation(SpriteClass& spriteClass, IndexReference indexReference, const Sprite& sprite) {
	if (indexReference.index < spriteClass.scaleAnimations.size()
		&& indexReference.generation == spriteClass.scaleAnimations[indexReference.index].spriteGeneration) {
		spriteClass.scaleAnimations[indexReference.index].flaotAnimation.done = true;
	}

	spriteClass.Buffer_scaleValueData[sprite.BufferIndex_scaleValueData] = spriteClass.defaultScale;
	spriteClass.BufferRefreshFlag_scaleValueData = true;
}

IndexReference queueScaleAnimation(SpriteClass& spriteClass, int spriteIndex, float from, float to, float duration, float startedAt) {
	FloatAnimation floatAnimation;
	floatAnimation.durationTicks = duration;
	floatAnimation.started = startedAt;
	floatAnimation.from = from;
	floatAnimation.to = to;

	for (int i = 0; i < spriteClass.scaleAnimations.size(); i++) {
		if (spriteClass.scaleAnimations[i].flaotAnimation.done) {
			spriteClass.scaleAnimations[i].flaotAnimation = floatAnimation;
			spriteClass.scaleAnimations[i].spriteGeneration++;
			spriteClass.scaleAnimations[i].spriteIndex = spriteIndex;
			IndexReference reference;
			reference.generation = spriteClass.scaleAnimations[i].spriteGeneration;
			reference.index = i;

			return reference;
		}
	}

	SpriteAnimation spriteAnimation;
	spriteAnimation.flaotAnimation = floatAnimation;
	spriteAnimation.spriteGeneration = 0;
	spriteAnimation.spriteIndex = spriteIndex;

	IndexReference reference;
	reference.generation = spriteAnimation.spriteGeneration;
	reference.index = spriteClass.scaleAnimations.size();

	spriteClass.scaleAnimations.push_back(spriteAnimation);

	return reference;
}
