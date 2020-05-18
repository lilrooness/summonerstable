#pragma once

#include <vector>
#include <GL/GLU.h>
#include "animation.h"
#include "sound.h"
#include "sprite_material.h"
#include "reference.h"
#include "sprite.h"

const float DEFUALT_MODEL_SCALE = 256.0f;

const float CARD_SPRITE_ROW = 1.0f / 16.0f * 5.0f;
const float CARD_SPRITE_HEIGHT = 1.0f / 16.0f;
const float CARD_SPRITE_WIDTH = 0.5f / 16.0f;

const float NEW_CANDLE_SPRITE_ROW = 1.0f/16.0f * 10.0f;
const float NEW_CANDLE_SPRITE_HEIGHT = 2.0f * (1.0f / 16.0f);
const float NEW_CANDLE_SPRITE_WIDTH = 1.0f * (1.0f / 16.0f);

const float SPELL_SPRITE_ROW = 1.0f / 16.0f * 6;
const float SPELL_SPRITE_HEIGHT = 1.0f / 16.0f;
const float SPELL_SPRITE_WIDTH = 1.0f / 16.0f;

const float ATTACK_SPRITE_SIZE = 0.03125f;
const float ATTACK_SPRITE_ROW = ATTACK_SPRITE_SIZE * 7;

const float SUMMON_CIRCLE_SPRITE_SIZE = 0.0625f;
const float SUMMON_CIRCLE_ROW = 0.0625f * 7.0f;

enum Suit { EYE, BONE, FLESH, BLOOD, HAIR };

enum CardAnimationType { SCALE };

const float CARD_WIDTH = 0.25f * 512.0f;
const float	CARD_HEIGHT = 0.5f * 512.0f;

struct Spell;
struct Game;
struct CardReference;
struct Card;
struct Attack;
struct Candle;
struct Stack;
struct StoredStack;
struct CardReference;

struct CardReference {
	int cardIndex;
	int generation;
};

struct CardAnimation {
	FloatAnimation animation;
	CardReference cardReference;
	int generation;
};

struct Spell {
	std::vector<Suit> requirements;
	void (*castSpell)(Game* game);
	bool mouseHovering = false;
	Sprite sprite;
};

struct Card {
	int number;
	Suit suit;
	bool deleted;
	int generation;
	bool mouseIsHovering{ false };
	Sprite sprite;
};

struct Attack {
	int number;
	bool deleted;
	int generation;
	int stackIndex;

	int BufferIndex_attackTextureOffsetData;
	int BufferIndex_attackVertexOffsetData;
	int BufferIndex_attackScaleValueData;
	int BufferIndex_attackTintValueData;
};

struct Candle {
	int BufferIndex_candleVertexOffsetData;
	int BufferIndex_candleTextureOffsetData;
	int BufferIndex_candleStateData;

	int burnLevel;
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
	int turn;
	int summonLevel = 0;
	float gameTime;
	bool rmbDown{ false }, lmbDown{ false };
	float mouseX{ 0.0f }, mouseY{ 0.0f };
	float lastMouseX{ 0.0f }, lastMouseY{ 0.0f };

	bool turnEndedByPlayer = false;

	CardReference grabbedCardReference;
	std::vector<Card> cards;
	std::vector<Stack> stacks;
	std::vector<CardReference> handCards;
	std::vector <Candle> candles;
	std::vector<Attack> attacks;
	std::vector <Spell> spells;

	int handLimit;

	SpriteClass cardSpriteClass;
	SpriteClass spellSpriteClass;

	std::vector<GLfloat> Buffer_attacksVertexOffsetData;
	std::vector<GLfloat> Buffer_attacksTextureOffsetData;
	std::vector<GLfloat> Buffer_attacksScaleValueData;
	std::vector<GLfloat> Buffer_attacksTintValueData;

	std::vector<GLfloat> Buffer_candlesVertexOffsetData;
	std::vector<GLfloat> Buffer_candlesTextureOffsetData;
	std::vector<GLfloat> Buffer_candlesStateData;

	std::vector<GLfloat> Buffer_circleVertexOffsetData;
	std::vector<GLfloat> Buffer_circleTextureOffsetData;
	std::vector<GLfloat> Buffer_circleTintValueData;
	std::vector<GLfloat> Buffer_circleScaleValueData;

	
	
	bool BufferRefreshFlag_numbersTextureOffsetData;
	
	bool BufferRefreshFlag_candlesVertexOffsetData;
	bool BufferRefreshFlag_candlesTextureOffsetData;
	bool BufferRefreshFlag_candlesStateData;
	
	bool BufferRefreshFlag_attacksVertexOffsetData;
	bool BufferRefreshFlag_attacksTextureOffsetData;
	bool BufferRefreshFlag_attacksScaleValueData;
	
	bool BufferRefreshFlag_circleStateData;
	bool BufferRefreshFlag_circleVertexOffsetData;
	bool BufferRefreshFlag_circleTextureOffsetData;
	bool BufferRefreshFlag_circleTintValueData;

	SoundState soundState;
};