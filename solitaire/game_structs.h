#pragma once

#include <vector>
#include <GL/GLU.h>
#include "animation.h"
#include "sound.h"
#include "sprite_material.h"
#include "reference.h"
#include "sprite.h"

const float DEFUALT_MODEL_SCALE = 256.0f;

const float SPELL_POPUP_SPRITE_ROW = 1.0f / 16.0f * 4.0f;
const float SPELL_POPUP_SPRITE_WIDTH = (1.0f / 16.0f) * 2.0f;
const float SPELL_POPUP_SPRITE_HEIGHT = (1.0f / 16.0f);

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
const float NULL_ATTACK_SPRITE_ROW = ATTACK_SPRITE_SIZE * 31;

const float SUMMON_CIRCLE_SPRITE_SIZE = 0.0625f;
const float SUMMON_CIRCLE_ROW = 0.0625f * 7.0f;

const float DUST_BOWL_SPRITE_SIZE = 1.0f / 16.0f;
const float DUST_BOWL_SPRITE_ROW = 0.0f;
const float DUST_BOWL_COLUMN = DUST_BOWL_SPRITE_SIZE * 11.0f;

enum Suit { EYE, BONE, FLESH, BLOOD, HAIR };

enum SpellType {SUMMON_SPELL, FLESH_SPELL, BONE_SPELL};

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
struct DustBowl;

struct DustBowl {
	Sprite sprite;
	int dustRemaining;
	bool mouseHovering = false;
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

struct Spell {
	SpellType type;
	bool mouseHovering = false;
	bool isCasting = false;
	IndexReference spellPopupReference;
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

struct SpellPopup {
	int generation;
	bool showing;
	Sprite sprite;
};

struct Attack {
	int number;
	int effectiveNumber;
	bool deleted;
	int generation;
	int stackIndex;

	Sprite sprite;
};

struct Candle {

	int burnLevel;

	Sprite sprite;
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
	bool rmbLifted{ false }, lmbLifted{ false };
	float mouseX{ 0.0f }, mouseY{ 0.0f };
	float lastMouseX{ 0.0f }, lastMouseY{ 0.0f };

	bool turnEndedByPlayer = false;

	int handLimit;

	int attackValueLimit;
	int attackLimit;

	CardReference grabbedCardReference;
	std::vector<Card> cards;
	std::vector<Stack> stacks;
	std::vector<CardReference> handCards;
	std::vector <Candle> candles;
	std::vector<Attack> attacks;
	std::vector <Spell> spells;
	std::vector <SpellPopup> spellPopups;

	DustBowl dustBowl;

	SpriteClass cardSpriteClass;
	SpriteClass spellSpriteClass;
	SpriteClass spellPopupSpriteClass;
	SpriteClass attacksSpriteClass;
	SpriteClass dustBowlSpriteClass;
	SpriteClass candlesSpriteClass;

	std::vector<GLfloat> Buffer_circleVertexOffsetData;
	std::vector<GLfloat> Buffer_circleTextureOffsetData;
	std::vector<GLfloat> Buffer_circleTintValueData;
	std::vector<GLfloat> Buffer_circleScaleValueData;
	
	bool BufferRefreshFlag_numbersTextureOffsetData;
	
	bool BufferRefreshFlag_circleStateData;
	bool BufferRefreshFlag_circleVertexOffsetData;
	bool BufferRefreshFlag_circleTextureOffsetData;
	bool BufferRefreshFlag_circleTintValueData;

	SoundState soundState;
};

void deleteCard(Game* game, CardReference cardReference) {
	if (cardReference.cardIndex < game->cards.size() && cardReference.generation == game->cards[cardReference.cardIndex].generation) {
		Card* card = &game->cards[cardReference.cardIndex];
		card->deleted = true;
		game->cardSpriteClass.Buffer_vertexOffsetData[card->sprite.BufferIndex_vertexOffsetData] = -3000;
	}
}

int createNewAttack(Game* game, int number, float x, float y, int stackIndex) {
	cout << "Creating new attack" << endl;
	Attack attack;
	attack.number = number;
	attack.effectiveNumber = number;
	attack.stackIndex = stackIndex;
	attack.deleted = false;
	attack.generation = 0;
	attack.sprite.BufferIndex_textureOffsetData = game->attacksSpriteClass.Buffer_textureOffsetData.size();
	attack.sprite.BufferIndex_vertexOffsetData = game->attacksSpriteClass.Buffer_vertexOffsetData.size();
	attack.sprite.BufferIndex_scaleValueData = game->attacksSpriteClass.Buffer_scaleValueData.size();
	attack.sprite.BufferIndex_tintValueData = game->attacksSpriteClass.Buffer_tintValueData.size();

	game->attacksSpriteClass.Buffer_textureOffsetData.push_back((number - 1) * ATTACK_SPRITE_SIZE);
	game->attacksSpriteClass.Buffer_textureOffsetData.push_back(ATTACK_SPRITE_ROW);

	game->attacksSpriteClass.Buffer_vertexOffsetData.push_back(x);
	game->attacksSpriteClass.Buffer_vertexOffsetData.push_back(y);
	game->attacksSpriteClass.Buffer_vertexOffsetData.push_back(0.0f);

	game->attacksSpriteClass.Buffer_scaleValueData.push_back(1.0f);
	game->attacksSpriteClass.Buffer_tintValueData.push_back(1.0f);
	game->attacksSpriteClass.Buffer_tintValueData.push_back(1.0f);
	game->attacksSpriteClass.Buffer_tintValueData.push_back(1.0f);

	int attackIndex = game->attacks.size();
	game->attacks.push_back(attack);

	return attackIndex;
}


IndexReference reuseOrCreateAttack(Game* game, int number, float x, float y, int stackIndex) {
	for (int i = 0; i < game->attacks.size(); i++) {
		if (game->attacks[i].deleted) {
			game->attacks[i].deleted = false;
			game->attacks[i].generation++;
			game->attacks[i].number = number;
			game->attacks[i].effectiveNumber = number;
			game->attacks[i].stackIndex = stackIndex;
			game->attacksSpriteClass.Buffer_vertexOffsetData[game->attacks[i].sprite.BufferIndex_vertexOffsetData] = x;
			game->attacksSpriteClass.Buffer_vertexOffsetData[game->attacks[i].sprite.BufferIndex_vertexOffsetData + 1] = y;
			game->attacksSpriteClass.Buffer_vertexOffsetData[game->attacks[i].sprite.BufferIndex_vertexOffsetData + 2] = 0.0f;

			game->attacksSpriteClass.Buffer_textureOffsetData[game->attacks[i].sprite.BufferIndex_textureOffsetData] = ATTACK_SPRITE_SIZE * (number - 1);
			game->attacksSpriteClass.Buffer_textureOffsetData[game->attacks[i].sprite.BufferIndex_textureOffsetData + 1] = ATTACK_SPRITE_ROW;

			game->attacksSpriteClass.Buffer_scaleValueData[game->attacks[i].sprite.BufferIndex_scaleValueData] = 1.0f;
			game->attacksSpriteClass.Buffer_tintValueData[game->attacks[i].sprite.BufferIndex_tintValueData] = 1.0f;
			game->attacksSpriteClass.Buffer_tintValueData[game->attacks[i].sprite.BufferIndex_tintValueData + 1] = 1.0f;
			game->attacksSpriteClass.Buffer_tintValueData[game->attacks[i].sprite.BufferIndex_tintValueData + 2] = 1.0f;

			IndexReference reference;
			reference.generation = game->attacks[i].generation;
			reference.index = i;

			return reference;
		}
	}

	int newAttackIndex = createNewAttack(game, number, x, y, stackIndex);
	IndexReference reference;
	reference.index = newAttackIndex;
	reference.generation = 0;

	return reference;
}