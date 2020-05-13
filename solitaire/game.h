#pragma once

#include <vector>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include "game_structs.h"
#include "card.h"

void init_game(Game *game);
void tick(Game *game, float mouseX, float mouseY, float dt);
bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void addCandles(Game* game);
int createNewCard(Game* game, Suit suit, int number, int stackIndex);
bool validCardReference(Game* game, const CardReference& cardReference);
Card* getCardByCardReference(Game* game, const CardReference& cardReference);
void pruneStacksCardReferences(Game* game);
void pruneHandCardReferences(Game* game);
int getCardStackIndex(Game* game, const CardReference& card);
void removeCardFromHand(Game* game, const CardReference& cardReference);
void removeCardFromStack(Game* game, const CardReference& cardReference, int stackIndex);
bool cardReferencesMatch(const CardReference& ref1, const CardReference& ref2);
int pickCard(Game* game, float mouseX, float mouseY);
void resetCardsAtStackPositions(Game* game);
bool cardFitsOnStack(Game* game, const Stack& stack, const CardReference& cardReference);
void endTurn(Game* game);
int countRemainingCandles(Game* game);
CardReference reuseOrCreateNewCard(Game *game, Suit suit, int number, float x, float y);
bool markCardAsDeleted(Game* game, const CardReference& cardReference);
void resolveCardScaleAnimations(Game* game);
IndexReference queueCardScalingAnimation(Game* game, CardAnimation cardScalingAnimation);
void cancelCardScalingAnimation(Game* game, const IndexReference& scalingAnimationReference);
bool validScalingRefereceAnimation(Game* game, const IndexReference& scalingAnimationReference);
CardAnimation* getCardAnimationByIndexReference(Game* game, const IndexReference& scalingAnimationReference);
void initCircle(Game* game);

void addAttacks(Game *game);
IndexReference reuseOrCreateAttack(Game* game, int number, float x, float y, int stackIndex);
int createNewAttack(Game* game, int number, float x, float y, int stackIndex);

void initCircle(Game* game) {
	GLfloat size = 300.0f;
	GLfloat textureOffsetX = 0.0f;

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			game->Buffer_circleVertexOffsetData.push_back((GLfloat)x * size + 425);//x
			game->Buffer_circleVertexOffsetData.push_back(((GLfloat)(3.0f-y)  * size));//y
			game->Buffer_circleVertexOffsetData.push_back((GLfloat)-1.0f);//z

			game->Buffer_circleTextureOffsetData.push_back((GLfloat)x * SUMMON_CIRCLE_SPRITE_SIZE);//x
			game->Buffer_circleTextureOffsetData.push_back((GLfloat)y * SUMMON_CIRCLE_SPRITE_SIZE + SUMMON_CIRCLE_ROW);//y
			game->Buffer_circleScaleValueData.push_back((GLfloat)1.0f);
			game->Buffer_circleTintValueData.push_back((GLfloat)1.0f);
		}
	}
}

IndexReference reuseOrCreateAttack(Game* game, int number, float x, float y, int stackIndex) {
	for (int i = 0; i < game->attacks.size(); i++) {
		if (game->attacks[i].deleted) {
			game->attacks[i].deleted = false;
			game->attacks[i].generation++;
			game->attacks[i].number = number;
			game->attacks[i].stackIndex = stackIndex;
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData] = x;
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData + 1] = y;
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData + 2] = 0.0f;

			game->Buffer_attacksTextureOffsetData[game->attacks[i].BufferIndex_attackTextureOffsetData] = ATTACK_SPRITE_SIZE * (number - 1);
			game->Buffer_attacksTextureOffsetData[game->attacks[i].BufferIndex_attackTextureOffsetData + 1] = ATTACK_SPRITE_ROW;

			game->Buffer_attacksScaleValueData[game->attacks[i].BufferIndex_attackScaleValueData] = 1.0f;
			game->Buffer_attacksTintValueData[game->attacks[i].BufferIndex_attackTintValueData] = 1.0f;

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

void addAttacks(Game* game) {
	int numberOfAttacks = (rand() % 4) + 1;
	float startingPosition = 265.0f;
	float gap = 350.0f;
	std::vector<int> availablePositions{ 0,1,2,3,4 };

	for (int i = 0; i < numberOfAttacks; i++) {
		int positionIndex = rand() % (availablePositions.size() - 1);
		int position = availablePositions[positionIndex];
		availablePositions.erase(availablePositions.begin() + positionIndex);
		int attackValue = (rand() % 4) + 1;
		// here we uise the position as the stack index, as they represent the same thing?
		reuseOrCreateAttack(game, attackValue, startingPosition + gap * position, 1250.0f, position);
	}
}

int createNewAttack(Game* game, int number, float x, float y, int stackIndex) {
	Attack attack;
	attack.number = number;
	attack.stackIndex = stackIndex;
	attack.deleted = false;
	attack.generation = 0;
	attack.BufferIndex_attackTextureOffsetData = game->Buffer_attacksTextureOffsetData.size();
	attack.BufferIndex_attackVertexOffsetData = game->Buffer_attacksVertexOffsetData.size();
	attack.BufferIndex_attackScaleValueData = game->Buffer_attacksScaleValueData.size();
	attack.BufferIndex_attackTintValueData = game->Buffer_attacksTintValueData.size();

	game->Buffer_attacksTextureOffsetData.push_back((number - 1) * ATTACK_SPRITE_SIZE);
	game->Buffer_attacksTextureOffsetData.push_back(ATTACK_SPRITE_ROW);
	
	game->Buffer_attacksVertexOffsetData.push_back(x);
	game->Buffer_attacksVertexOffsetData.push_back(y);
	game->Buffer_attacksVertexOffsetData.push_back(0.0f);

	game->Buffer_attacksScaleValueData.push_back(1.0f);
	game->Buffer_attacksTintValueData.push_back(1.0f);

	int attackIndex = game->attacks.size();
	game->attacks.push_back(attack);

	return attackIndex;
}

void cancelCardScalingAnimation(Game* game, const IndexReference& scalingAnimationReference) {
	if (validScalingRefereceAnimation(game, scalingAnimationReference)) {
		CardAnimation* cardAnimation = getCardAnimationByIndexReference(game, scalingAnimationReference);
		cardAnimation->animation.done = true;
	}
}

CardAnimation* getCardAnimationByIndexReference(Game* game, const IndexReference& scalingAnimationReference) {
	if (validScalingRefereceAnimation(game, scalingAnimationReference)) {
		return &game->cardScalingAnimations[scalingAnimationReference.index];
	}

	return nullptr;
}

bool validScalingRefereceAnimation(Game* game, const IndexReference& scalingAnimationReference) {
	if (scalingAnimationReference.index >= game->cardScalingAnimations.size()) {
		return false;
	}
	
	if (scalingAnimationReference.generation != game->cardScalingAnimations[scalingAnimationReference.index].generation) {
		return false;
	}
	
	return true;
}

IndexReference queueCardScalingAnimation(Game* game, CardAnimation cardScalingAnimation) {
	for (int i = 0; i < game->cardScalingAnimations.size(); i++) {
		if (!validCardReference(game, game->cardScalingAnimations[i].cardReference) || game->cardScalingAnimations[i].animation.done) {
			int currentGeneration = game->cardScalingAnimations[i].generation;
			game->cardScalingAnimations[i] = cardScalingAnimation;
			game->cardScalingAnimations[i].generation = currentGeneration + 1;
			IndexReference ref;
			ref.index = i;
			ref.generation = game->cardScalingAnimations[i].generation;
			return ref;
		}
	}

	cardScalingAnimation.generation = 0;
	int newIndex = game->cardScalingAnimations.size();
	game->cardScalingAnimations.push_back(cardScalingAnimation);

	IndexReference indexReference;
	indexReference.generation = cardScalingAnimation.generation;
	indexReference.index = newIndex;
	
	return indexReference;
}

void resolveCardScaleAnimations(Game* game) {
	for (CardAnimation& cardAnimation : game->cardScalingAnimations) {
		if (!cardAnimation.animation.done && validCardReference(game, cardAnimation.cardReference)) {
			float currentAnimationValue = getCurrentAnimationValue(cardAnimation.animation, game->gameTime);
			Card* card = getCardByCardReference(game, cardAnimation.cardReference);
			game->Buffer_cardsScaleValueData[card->BufferIndex_cardScaleValueData] = currentAnimationValue;
			game->BufferRefreshFlag_cardsScaleValueData = true;
		}
		else {
			cardAnimation.animation.done = true;
		}
	}
}

bool markCardAsDeleted(Game* game, const CardReference& cardReference) {

	if (validCardReference(game, cardReference)) {
		Card *card = getCardByCardReference(game, cardReference);
		card->deleted = true;
		game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData] = -300;
		game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 1] = -300;
		return true;
	}

	return false;
}

IndexReference reuseOrCreateNewAttack(Game* game, int number, float x, float y, int stackIndex) {
	for (int i = 0; i < game->attacks.size(); i++) {
		if (game->attacks[i].deleted) {
			game->attacks[i].deleted = false;
			game->attacks[i].generation++;
			game->attacks[i].number = number;
			game->attacks[i].stackIndex = stackIndex;
			
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData] = x;
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData + 1] = y;
			game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData + 2] = 0.0f;

			game->Buffer_attacksTextureOffsetData[game->attacks[i].BufferIndex_attackTextureOffsetData] = (number - 1) * ATTACK_SPRITE_SIZE;
			game->Buffer_attacksTextureOffsetData[game->attacks[i].BufferIndex_attackTextureOffsetData] = 7.0f * ATTACK_SPRITE_SIZE;
		
			game->Buffer_attacksScaleValueData[game->attacks[i].BufferIndex_attackScaleValueData] = 1.0f;
			
			IndexReference reference;
			reference.generation = game->attacks[i].generation;
			reference.index = i;

			return reference;
		}
	}

	int index = createNewAttack(game, number, x, y, stackIndex);
	IndexReference reference;
	reference.index = index;
	reference.generation = 0;

	return reference;
}

CardReference reuseOrCreateNewCard(Game* game, Suit suit, int number, float x, float y) {
	for (int i = 0; i < game->cards.size(); i++) {
		if (game->cards[i].deleted) {
			game->cards[i].generation += 1;
			game->cards[i].deleted = false;
			game->cards[i].suit = suit;
			game->cards[i].number = number;

			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData] = 0.0f;
			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData + 1] = 0.0f;
			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData + 2] = 1.0f;

			game->Buffer_cardsTextureOffsetData[game->cards[i].BufferIndex_cardTextureOffsetData] = (GLfloat)suit * (GLfloat)0.125f;
			game->Buffer_cardsTextureOffsetData[game->cards[i].BufferIndex_cardTextureOffsetData + 1] = (GLfloat)0.0f;

			CardReference reference;
			reference.cardIndex = i;
			reference.generation = game->cards[i].generation;

			return reference;
		}
	}

	int cardIndex = createNewCard(game, suit, number, -1);

	CardReference reference;
	reference.generation = game->cards[cardIndex].generation;
	reference.cardIndex = cardIndex;
}

void offsetCandleTexturesToMatchBurnLevels(Game* game) {
	for (const Candle& candle : game->candles) {
		game->Buffer_candlesTextureOffsetData[candle.BufferIndex_candleTextureOffsetData] = NEW_CANDLE_SPRITE_WIDTH * candle.burnLevel;
	}

	game->BufferRefreshFlag_candlesTextureOffsetData = true;
}

void endTurn(Game* game) {
	for (int i = 0; i < game->attacks.size(); i++) {
		if (!game->attacks[i].deleted && game->attacks[i].number > game->stacks[game->attacks[i].stackIndex].orderedCardReferences.size()) {
			// do the thing where we decrease the stack's candle size...
			game->candles[game->attacks[i].stackIndex].burnLevel += game->attacks[i].number - game->stacks[game->attacks[i].stackIndex].orderedCardReferences.size();
			offsetCandleTexturesToMatchBurnLevels(game);
		}
		game->attacks[i].deleted = true;
		game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData] = -300;
		game->Buffer_attacksVertexOffsetData[game->attacks[i].BufferIndex_attackVertexOffsetData + 1] = -300;
	}
	addAttacks(game);
	game->BufferRefreshFlag_attacksTextureOffsetData = true;
	game->BufferRefreshFlag_attacksVertexOffsetData = true;
	game->BufferRefreshFlag_attacksScaleValueData = true;
	

	for (Stack& stack : game->stacks) {
		for (const CardReference& cardReference : stack.orderedCardReferences) {
			if (validCardReference(game, cardReference)) {
				markCardAsDeleted(game, cardReference);
			}
		}
	}

	pruneStacksCardReferences(game);

	int cardsToDeal = 4;

	for (int i = 0; i < cardsToDeal; i++) {
		Suit cardSuit = static_cast<Suit>(rand() % 5);
		int number = (rand() % 10) + 1;
		CardReference cardReference = reuseOrCreateNewCard(game, cardSuit, number, 0.0f, 0.0f);
		game->stacks[i].orderedCardReferences.push_back(cardReference);
	}

	game->BufferRefreshFlag_cardsTextureOffsetData = true;
	game->BufferRefreshFlag_numbersTextureOffsetData = true;
	game->BufferRefreshFlag_cardsVertexOffsetData = true;

	game->turnEndedByPlayer = false;
	game->turn++;

	int track;
	if (game->turn % 2 == 0) {
		track = 1;
	} else  {
		track = 0;
	}
	playMusic(&game->soundState, track);
}

int countRemainingCandles(Game* game) {
	int count{ 0 };
	for (GLfloat i : game->Buffer_candlesStateData) {
		if (i == 1.0f) {
			count++;
		}
	}

	return count;
}

void init_game(Game *game) {
	initSound(&game->soundState);
	game->turn = 1;
	srand(time(NULL));
	game->gameTime = 0.0f;
	//table stacks
	Stack stack1{ 200.0f + 50, 550.0f };
	Stack stack2{ 550.0f + 50, 550.0f };
	Stack stack3{ 900.0f + 50, 550.0f };
	Stack stack4{ 1250.0f + 50, 550.0f };

	game->handLimit = 5;

	game->stacks.push_back(stack1);
	game->stacks.push_back(stack2);
	game->stacks.push_back(stack3);
	game->stacks.push_back(stack4);

	game->grabbedCardReference = CardReference{ -1, -1 };

	for (int i = 0; i < game->stacks.size(); i++) {
		Suit cardSuit = static_cast<Suit>( rand() % 5);
		int number = (rand() % 10) + 1;
		createNewCard(game, cardSuit, number, i);
	}

	addCandles(game);
	addAttacks(game);
	initCircle(game);

	game->BufferRefreshFlag_cardsVertexOffsetData = false;
	game->BufferRefreshFlag_cardsTextureOffsetData = false;
	game->BufferRefreshFlag_numbersTextureOffsetData = false;
	game->BufferRefreshFlag_candlesVertexOffsetData = false;
	game->BufferRefreshFlag_candlesTextureOffsetData = false;
	game->BufferRefreshFlag_candlesStateData = false;

	playMusic(&game->soundState, 0);
}

void tick(Game *game, float mouseX, float mouseY, float dt) {

	game->gameTime += dt;
	resolveCardScaleAnimations(game);

	game->lastMouseX = game->mouseX;
	game->lastMouseY = game->mouseY;
	game->mouseX = mouseX;
	game->mouseY = mouseY;

	game->BufferRefreshFlag_cardsVertexOffsetData = false;
	game->BufferRefreshFlag_cardsTextureOffsetData = false;
	game->BufferRefreshFlag_numbersTextureOffsetData = false;
	game->BufferRefreshFlag_candlesVertexOffsetData = false;
	game->BufferRefreshFlag_candlesTextureOffsetData = false;
	game->BufferRefreshFlag_candlesStateData = false;

	if (game->turnEndedByPlayer) {
		endTurn(game);
	}

	if (game->lmbDown && validCardReference(game, game->grabbedCardReference)) {
		GLfloat xDiff = game->mouseX - game->lastMouseX;
		GLfloat yDiff = game->mouseY - game->lastMouseY;

		Card* grabbedCard = getCardByCardReference(game, game->grabbedCardReference);
		game->Buffer_cardsVertexOffsetData[grabbedCard->BufferIndex_cardVertexOffsetData] += xDiff;
		game->Buffer_cardsVertexOffsetData[grabbedCard->BufferIndex_cardVertexOffsetData + 1] += yDiff;
		game->BufferRefreshFlag_cardsVertexOffsetData = true;
	}

	//if card is grabbed but LMB is not down, release the card
	if (!game->lmbDown && validCardReference(game, game->grabbedCardReference)) {
		Card *grabbedCard = getCardByCardReference(game, game->grabbedCardReference);
		GLfloat grabbedX = game->Buffer_cardsVertexOffsetData[grabbedCard->BufferIndex_cardVertexOffsetData];
		GLfloat grabbedY = game->Buffer_cardsVertexOffsetData[grabbedCard->BufferIndex_cardVertexOffsetData + 1];
		
		//reset card Z position
		Card* card = getCardByCardReference(game, game->grabbedCardReference);
		game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 2] = 1.0f;

		//attempt to put the card in the hand
		if (grabbedY < 50.0f) {
			if (game->handCards.size() < game->handLimit) {
				int originalStackIndex = getCardStackIndex(game, game->grabbedCardReference);

				/*
					if it was in a stack, remove it and put it in the hand
					otherwise, assume that It was in the hand before it was picked up
					so we dont need to do anything, as the card will be droppped anyway
				*/
				if (originalStackIndex > -1) {
					removeCardFromStack(game, game->grabbedCardReference, originalStackIndex);
					game->handCards.push_back(game->grabbedCardReference);
				}
			}
		}
		//attempt to put the card down on a stack
		else {
			int originalStackIndex = getCardStackIndex(game, game->grabbedCardReference);
			for (Stack& stack : game->stacks) {
				if (boxCollision(grabbedX, grabbedY, CARD_WIDTH, CARD_HEIGHT, stack.x, stack.y, CARD_WIDTH, CARD_HEIGHT) && cardFitsOnStack(game, stack, game->grabbedCardReference)) {
					//remove card from previous stack and place in new stack (even if they are the same stack)
					if (originalStackIndex > -1) {
						removeCardFromStack(game, game->grabbedCardReference, originalStackIndex);
						stack.orderedCardReferences.push_back(game->grabbedCardReference);
					}
					//Card wasn't in a stack, meaning that it came from the hand. Remove card from hand and place in new stack
					else {
						removeCardFromHand(game, game->grabbedCardReference);
						stack.orderedCardReferences.push_back(game->grabbedCardReference);
					}
				}
			}
		}

		game->grabbedCardReference.cardIndex = -1;
		game->grabbedCardReference.generation = -1;
	}

	int pickedCardIndex = pickCard(game, game->mouseX, game->mouseY);

	for (int i = 0; i < game->cards.size(); i++) {
		if (game->cards[i].mouseIsHovering && i != pickedCardIndex) {
			game->cards[i].mouseIsHovering = false;
			cancelCardScalingAnimation(game, game->cards[i].hoverAnimationReference);
			game->Buffer_cardsScaleValueData[game->cards[i].BufferIndex_cardScaleValueData] = 1.0f;
			game->BufferRefreshFlag_cardsScaleValueData = true;
		}
	}
	
	if (pickedCardIndex > -1 && !game->cards[pickedCardIndex].mouseIsHovering) {
		game->cards[pickedCardIndex].mouseIsHovering = true;
		FloatAnimation animation = createAnimation(1.0f, 1.1f, 5.0f, game->gameTime);
		CardReference cardReference;
		cardReference.cardIndex = pickedCardIndex;
		cardReference.generation = game->cards[pickedCardIndex].generation;
		CardAnimation cardAnimation;
		cardAnimation.cardReference = cardReference;
		cardAnimation.animation = animation;
		IndexReference animationReference = queueCardScalingAnimation(game, cardAnimation);
		game->cards[pickedCardIndex].hoverAnimationReference = animationReference;
	}

	if (game->lmbDown && !validCardReference(game, game->grabbedCardReference) && pickedCardIndex > -1) {
		CardReference cardReference;
		cardReference.cardIndex = pickedCardIndex;
		cardReference.generation = game->cards[pickedCardIndex].generation;
		game->grabbedCardReference = cardReference;

		//raise card Z posision
		Card* card = getCardByCardReference(game, cardReference);
		game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 2] = 2.0f;
	}

	pruneStacksCardReferences(game);
	pruneHandCardReferences(game);

	resetCardsAtStackPositions(game);
}

bool cardFitsOnStack(Game *game, const Stack& stack, const CardReference& cardReference) {
	if (stack.orderedCardReferences.empty()) {
		return true;
	}

	const CardReference& topCardReference = stack.orderedCardReferences[stack.orderedCardReferences.size() - 1];
	Card *topCard = getCardByCardReference(game, topCardReference);
	Card* card = getCardByCardReference(game, cardReference);
	
	if (topCard != nullptr && card != nullptr && topCard->suit == card->suit) {
		return true;
	}
	else {
		return false;
	}
}

void resetCardsAtStackPositions(Game* game) {
	float zIncrement = 0.1f;
	for (const Stack& stack : game->stacks) {
		for (int i = 0; i < stack.orderedCardReferences.size(); i++) {
			bool cardGrabbed = cardReferencesMatch(stack.orderedCardReferences[i], game->grabbedCardReference);
			if (!cardGrabbed) {
				Card* card = getCardByCardReference(game, stack.orderedCardReferences[i]);
				game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData] = stack.x;
				game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 1] = stack.y - (i * 10);
				game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 2] = (i+1) * zIncrement;
			}
		}
	}

	game->BufferRefreshFlag_cardsVertexOffsetData = true;
}
 
int pickCard(Game *game, float mouseX, float mouseY) {
	std::vector<int> collidedCardIndexes;
	for (int i = 0; i < game->cards.size(); i++) {
		int vertexBufferIndex = game->cards[i].BufferIndex_cardVertexOffsetData;
		float x{ game->Buffer_cardsVertexOffsetData[vertexBufferIndex] };
		float y{ game->Buffer_cardsVertexOffsetData[vertexBufferIndex + 1] };
		if (mouseX > x && mouseX < x + CARD_WIDTH && mouseY > y && mouseY < y + CARD_HEIGHT && !game->cards[i].deleted) {
			collidedCardIndexes.push_back(i);
		}
	}

	if (collidedCardIndexes.empty()) {
		return -1;
	}
	else {
		//sort the cards by their Z value and pick the top one
		std::sort(collidedCardIndexes.begin(), collidedCardIndexes.end(), [game](int indexA, int indexB) {
			float zA = game->Buffer_cardsVertexOffsetData[game->cards[indexA].BufferIndex_cardVertexOffsetData + 2];
			float zB = game->Buffer_cardsVertexOffsetData[game->cards[indexB].BufferIndex_cardVertexOffsetData + 2];
			return zA > zB;
		});
		return collidedCardIndexes[0];
	}
	
}

void removeCardFromHand(Game* game, const CardReference& cardReference) {
	auto checker = [=](const CardReference& cardRefIt) {
		return validCardReference(game, cardReference) && validCardReference(game, cardRefIt) && cardReferencesMatch(cardReference, cardRefIt);
	};

	auto it = std::find_if(game->handCards.begin(), game->handCards.end(), checker);

	game->handCards.erase(it);
}

void removeCardFromStack(Game* game, const CardReference& cardReference, int stackIndex) {
	auto checker = [=](const CardReference& cardRefIt) {
		return validCardReference(game, cardReference) && validCardReference(game, cardRefIt) && cardReferencesMatch(cardReference, cardRefIt);
	};

	auto it = std::find_if(game->stacks[stackIndex].orderedCardReferences.begin(), game->stacks[stackIndex].orderedCardReferences.end(), checker);

	game->stacks[stackIndex].orderedCardReferences.erase(it);
}

int getCardStackIndex(Game *game, const CardReference& cardReference) {
	for (int i = 0; i < game->stacks.size(); i++) {
		auto checker = [=](const CardReference& cardRefIt) {
			return validCardReference(game, cardReference) && validCardReference(game, cardRefIt) && cardReferencesMatch(cardReference, cardRefIt);
		};
		if (std::find_if(game->stacks[i].orderedCardReferences.begin(), game->stacks[i].orderedCardReferences.end(), checker) != game->stacks[i].orderedCardReferences.end()) {
			return i;
		}
	}

	return -1;
}

void pruneStacksCardReferences(Game* game) {
	for (Stack& stack : game->stacks) {
		
		auto it = stack.orderedCardReferences.begin();

		while (it != stack.orderedCardReferences.end()) {
			if (!validCardReference(game, (CardReference)*it)) {
				it = stack.orderedCardReferences.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

void pruneHandCardReferences(Game* game) {
	auto it = game->handCards.begin();

	while (it != game->handCards.end()) {
		if (!validCardReference(game, (CardReference)*it)) {
			it = game->handCards.erase(it);
		}
		else {
			++it;
		}
	}
}

Card* getCardByCardReference(Game* game, const CardReference& cardReference) {
	if (validCardReference(game, cardReference)) {
		return &game->cards[cardReference.cardIndex];
	}

	return nullptr;
}

bool validCardReference(Game* game, const CardReference& cardReference) {
	if (cardReference.cardIndex < 0 || cardReference.generation < 0 || cardReference.cardIndex >= game->cards.size()) {
		return false;
	}

	if (game->cards[cardReference.cardIndex].deleted) {
		return false;
	}

	if (game->cards[cardReference.cardIndex].generation == cardReference.generation ) {
		return true;
	}
	else {
		return false;
	}
}

bool cardReferencesMatch(const CardReference& ref1, const CardReference& ref2) {
	return (ref1.generation == ref2.generation && ref1.cardIndex == ref2.cardIndex);
}

int createNewCard(Game* game, Suit suit, int number, int stackIndex) {
	Card card;
	card.generation = 0;
	card.deleted = false;
	card.number = number;
	card.suit = suit;
	card.BufferIndex_cardVertexOffsetData = game->Buffer_cardsVertexOffsetData.size();
	GLfloat x, y, z{ 1.0f };
	if (stackIndex > -1) {
		x = game->stacks[stackIndex].x;
		y = game->stacks[stackIndex].y;
	}
	else {
		x = 0.0f;
		y = 0.0f;
	}

	game->Buffer_cardsVertexOffsetData.push_back(x);
	game->Buffer_cardsVertexOffsetData.push_back(y);
	game->Buffer_cardsVertexOffsetData.push_back(z);

	card.BufferIndex_cardTextureOffsetData = game->Buffer_cardsTextureOffsetData.size();
	game->Buffer_cardsTextureOffsetData.push_back(((GLfloat)suit) * (GLfloat)0.125f);
	game->Buffer_cardsTextureOffsetData.push_back((GLfloat)0.0f);

	card.BufferIndex_cardScaleValueData = game->Buffer_cardsScaleValueData.size();
	game->Buffer_cardsScaleValueData.push_back(1.0f);

	int cardIndex = game->cards.size();
	game->cards.push_back(card);

	if (stackIndex > -1) {
		game->stacks[stackIndex].orderedCardReferences.push_back(CardReference{
			cardIndex, card.generation });
	}

	return cardIndex;
}

bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	float x1Center = x1 + w1 / 2;
	float y1Center = y1 + h1 / 2;

	float x2Center = x2 + w2 / 2;
	float y2Center = y2 + h2 / 2;

	return (abs(x1Center - x2Center) < ((w1 + w2) / 2) && abs(y1Center - y2Center) < ((h1 + h2) / 2));
}

void addCandles(Game* game) {
	Candle candle1;
	candle1.burnLevel = 0;
	candle1.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(200.0f);
	game->Buffer_candlesVertexOffsetData.push_back(800);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle1.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	game->Buffer_candlesTextureOffsetData.push_back(NEW_CANDLE_SPRITE_ROW);
	candle1.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle2;
	candle2.burnLevel = 0;
	candle2.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(550.0f);
	game->Buffer_candlesVertexOffsetData.push_back(800);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle2.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	game->Buffer_candlesTextureOffsetData.push_back(NEW_CANDLE_SPRITE_ROW);
	candle2.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle3;
	candle3.burnLevel = 0;
	candle3.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(900.0f);
	game->Buffer_candlesVertexOffsetData.push_back(800);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle3.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	game->Buffer_candlesTextureOffsetData.push_back(NEW_CANDLE_SPRITE_ROW);
	candle3.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle4;
	candle4.burnLevel = 0;
	candle4.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(1250.0f);
	game->Buffer_candlesVertexOffsetData.push_back(800);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle4.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	game->Buffer_candlesTextureOffsetData.push_back(NEW_CANDLE_SPRITE_ROW);
	candle4.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	game->candles.push_back(candle1);
	game->candles.push_back(candle2);
	game->candles.push_back(candle3);
	game->candles.push_back(candle4);

}
