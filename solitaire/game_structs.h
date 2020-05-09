#pragma once

#include <vector>
#include <GL/GLU.h>
#include "animation.h"

float ATTACK_SPRITE_SIZE = 0.03125f;
float ATTACK_SPRITE_ROW = ATTACK_SPRITE_SIZE * 7;
float CANDLE_SPRITE_SIZE = 0.0625f;

enum Suit { EYE, BONE, FLESH, BLOOD, HAIR };

enum CardAnimationType { SCALE };

const float CARD_WIDTH = 0.25f * 512.0f;
const float	CARD_HEIGHT = 0.5f * 512.0f;

struct IndexReference {
	int index;
	int generation;
};

struct CardReference {
	int cardIndex;
	int generation;
};

struct CardAnimation {
	FloatAnimation animation;
	CardReference cardReference;
	int generation;
};

struct Card {
	int number;
	Suit suit;
	bool deleted;
	int generation;
	bool mouseIsHovering{ false };
	IndexReference hoverAnimationReference;

	int BufferIndex_cardTextureOffsetData;
	int BufferIndex_cardVertexOffsetData;
	int BufferIndex_cardScaleValueData;
	int BufferIndex_numberTextureOffsetData;
};

struct Attack {
	int number;
	bool deleted;
	int generation;
	int stackIndex;

	int BufferIndex_attackTextureOffsetData;
	int BufferIndex_attackVertexOffsetData;
	int BufferIndex_attackScaleValueData;
};

struct Candle {
	int BufferIndex_candleVertexOffsetData;
	int BufferIndex_candleTextureOffsetData;
	int BufferIndex_candleStateData;
};

struct Stack {
	float x, y;
	std::vector<CardReference> orderedCardReferences;
};

struct StoredStack {
	float x, y;
	std::vector<CardReference> storedCardReferences;
};

struct Game {
	float gameTime;
	bool rmbDown{ false }, lmbDown{ false };
	float mouseX{ 0.0f }, mouseY{ 0.0f };
	float lastMouseX{ 0.0f }, lastMouseY{ 0.0f };

	bool turnEndedByPlayer = false;

	CardReference grabbedCardReference;
	std::vector<Card> cards;
	std::vector<Stack> stacks;
	std::vector<CardReference> handCards;

	int handLimit;

	std::vector<GLfloat> Buffer_cardsVertexOffsetData;
	std::vector<GLfloat> Buffer_cardsTextureOffsetData;
	std::vector<GLfloat> Buffer_cardsScaleValueData;

	std::vector<GLfloat> Buffer_attacksVertexOffsetData;
	std::vector<GLfloat> Buffer_attacksTextureOffsetData;
	std::vector<GLfloat> Buffer_attacksScaleValueData;

	std::vector<GLfloat> Buffer_numbersTextureOffsetData;

	std::vector<GLfloat> Buffer_candlesVertexOffsetData;
	std::vector<GLfloat> Buffer_candlesTextureOffsetData;
	std::vector<GLfloat> Buffer_candlesStateData;

	std::vector<CardAnimation> cardScalingAnimations;

	std::vector<Attack> attacks;

	bool BufferRefreshFlag_cardsVertexOffsetData;
	bool BufferRefreshFlag_cardsTextureOffsetData;
	bool BufferRefreshFlag_numbersTextureOffsetData;
	bool BufferRefreshFlag_candlesVertexOffsetData;
	bool BufferRefreshFlag_candlesTextureOffsetData;
	bool BufferRefreshFlag_candlesStateData;
	bool BufferRefreshFlag_cardsScaleValueData;
	bool BufferRefreshFlag_attacksVertexOffsetData;
	bool BufferRefreshFlag_attacksTextureOffsetData;
	
};