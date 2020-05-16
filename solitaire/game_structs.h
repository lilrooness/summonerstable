#pragma once

#include <vector>
#include <GL/GLU.h>
#include "animation.h"
#include "sound.h"

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
struct IndexReference;
struct CardReference;
struct Card;
struct Attack;
struct Candle;
struct Stack;
struct StoredStack;

struct Spell {
	int BufferIndex_spellVertexOffsetData;
	int BufferIndex_spellTextureOffsetData;
	int BufferIndex_spellScaleValueData;
	int BufferIndex_spellTintValueData;
	std::vector<Suit> requirements;
	void (*castSpell)(Game* game);
	bool mouseHovering = false;
};

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
	std::vector <Spell> spells;

	int handLimit;

	std::vector<GLfloat> Buffer_cardsVertexOffsetData;
	std::vector<GLfloat> Buffer_cardsTextureOffsetData;
	std::vector<GLfloat> Buffer_cardsScaleValueData;

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

	std::vector<GLfloat> Buffer_spellsVertexOffsetData;
	std::vector<GLfloat> Buffer_spellsScaleValueData;
	std::vector<GLfloat> Buffer_spellsTextureOffsetData;
	std::vector<GLfloat> Buffer_spellsTintValueData;

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
	bool BufferRefreshFlag_attacksScaleValueData;
	
	bool BufferRefreshFlag_circleStateData;
	bool BufferRefreshFlag_circleVertexOffsetData;
	bool BufferRefreshFlag_circleTextureOffsetData;

	bool BufferRefreshFlag_spellsVertexOffsetData;
	bool BufferRefreshFlag_spellsTextureOffsetData;
	bool BufferRefreshFlag_spellsScaleValueData;
	bool BufferRefreshFlag_spellsTintValueData;

	
	SoundState soundState;
};