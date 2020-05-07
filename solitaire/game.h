#pragma once

#include <vector>
#include <GL/GLU.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>

//TODO: Check that these functions actually work:
// - void pruneStacksCardReferences(Game* game);
// - void pruneHandCardReferences(Game* game);

enum Suit { EYE, BONE, FLESH, BLOOD, HAIR};

const float CARD_WIDTH = 0.25f * 512.0f;
const float	CARD_HEIGHT = 0.5f * 512.0f;

struct CardReference {
	int cardIndex;
	int generation;
};

struct Card {
	int number;
	Suit suit;
	bool deleted;
	int generation;

	int BufferIndex_cardTextureOffsetData;
	int BufferIndex_cardVertexOffsetData;
	int BufferIndex_numberTextureOffsetData;
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

struct Game {
	bool rmbDown{ false}, lmbDown{ false };
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
	std::vector<GLfloat> Buffer_numbersTextureOffsetData;
	std::vector<GLfloat> Buffer_candlesVertexOffsetData;
	std::vector<GLfloat> Buffer_candlesTextureOffsetData;
	std::vector<GLfloat> Buffer_candlesStateData;

	bool BufferRefreshFlag_cardsVertexOffsetData;
	bool BufferRefreshFlag_cardsTextureOffsetData;
	bool BufferRefreshFlag_numbersTextureOffsetData;
	bool BufferRefreshFlag_candlesVertexOffsetData;
	bool BufferRefreshFlag_candlesTextureOffsetData;
	bool BufferRefreshFlag_candlesStateData;
};
//
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

CardReference reuseOrCreateNewCard(Game* game, Suit suit, int number, float x, float y) {
	for (int i = 0; i < game->cards.size(); i++) {
		if (game->cards[i].deleted) {
			game->cards[i].generation += 1;
			game->cards[i].deleted = false;
			game->cards[i].suit = suit;
			game->cards[i].number = number;

			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData] = 0.0f;
			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData + 1] = 0.0f;
			game->Buffer_cardsVertexOffsetData[game->cards[i].BufferIndex_cardVertexOffsetData + 2] = 0.0f;

			game->Buffer_cardsTextureOffsetData[game->cards[i].BufferIndex_cardTextureOffsetData] = (GLfloat)suit * (GLfloat)0.125f;
			game->Buffer_cardsTextureOffsetData[game->cards[i].BufferIndex_cardTextureOffsetData + 1] = (GLfloat)0.0f;

			game->Buffer_numbersTextureOffsetData[game->cards[i].BufferIndex_numberTextureOffsetData] = (GLfloat)number * (GLfloat)0.1f;
			game->Buffer_numbersTextureOffsetData[game->cards[i].BufferIndex_numberTextureOffsetData + 1] = (GLfloat)0.0f;

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

void endTurn(Game* game) {
	int candlesRemaining = countRemainingCandles(game);
	for (Stack& stack : game->stacks) {
		if (stack.orderedCardReferences.size() == 1) {
			candlesRemaining = max(0, candlesRemaining - 1);
		}
	}

	for (int i = 0; i < game->Buffer_candlesStateData.size(); i++) {
		if (candlesRemaining - i > 0) {
			game->Buffer_candlesStateData[i] = 1;
		}
		else {
			game->Buffer_candlesStateData[i] = 0;
		}
	}
	game->BufferRefreshFlag_candlesStateData = true;

	for (Stack& stack : game->stacks) {
		for (const CardReference& cardReference : stack.orderedCardReferences) {
			if (validCardReference(game, cardReference)) {
				markCardAsDeleted(game, cardReference);
			}
		}
	}
	pruneStacksCardReferences(game);

	int cardsToDeal = 5;

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
	srand(time(NULL));
	//table stacks
	Stack stack1{ 800.0f, 900 };
	Stack stack2{ 400.0f, 600.0f };
	Stack stack3{ 600.0f, 200.0f };
	Stack stack4{ 1000.0f, 200.0f };
	Stack stack5{ 1200.0f, 600.0f };

	game->handLimit = 5;

	game->stacks.push_back(stack1);
	game->stacks.push_back(stack2);
	game->stacks.push_back(stack3);
	game->stacks.push_back(stack4);
	game->stacks.push_back(stack5);

	game->grabbedCardReference = CardReference{ -1, -1 };

	for (int i = 0; i < game->stacks.size(); i++) {
		Suit cardSuit = static_cast<Suit>( rand() % 5);
		int number = (rand() % 10) + 1;
		createNewCard(game, cardSuit, number, i);
	}

	addCandles(game);

	game->BufferRefreshFlag_cardsVertexOffsetData = false;
	game->BufferRefreshFlag_cardsTextureOffsetData = false;
	game->BufferRefreshFlag_numbersTextureOffsetData = false;
	game->BufferRefreshFlag_candlesVertexOffsetData = false;
	game->BufferRefreshFlag_candlesTextureOffsetData = false;
	game->BufferRefreshFlag_candlesStateData = false;
}

void tick(Game *game, float mouseX, float mouseY, float dt) {

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
	if (game->lmbDown && !validCardReference(game, game->grabbedCardReference) && pickedCardIndex > -1) {
		CardReference cardReference;
		cardReference.cardIndex = pickedCardIndex;
		cardReference.generation = game->cards[pickedCardIndex].generation;
		game->grabbedCardReference = cardReference;
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
				game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 1] = stack.y;
				game->Buffer_cardsVertexOffsetData[card->BufferIndex_cardVertexOffsetData + 2] = i * zIncrement;
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
				++it;
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
	GLfloat x, y, z{ 0.0f };
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

	card.BufferIndex_numberTextureOffsetData = game->Buffer_numbersTextureOffsetData.size();
	game->Buffer_numbersTextureOffsetData.push_back(((GLfloat)number) * (GLfloat)0.1f);
	game->Buffer_numbersTextureOffsetData.push_back((GLfloat)0.0f);

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
	candle1.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(500.0f);
	game->Buffer_candlesVertexOffsetData.push_back(900);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle1.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(1 * 64.0f);
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	candle1.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle2;
	candle2.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(1000.0f);
	game->Buffer_candlesVertexOffsetData.push_back(900);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle2.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(2 * 64.0f);
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	candle2.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle3;
	candle3.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(300.0f);
	game->Buffer_candlesVertexOffsetData.push_back(300);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle3.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(3 * 64.0f);
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	candle3.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle4;
	candle4.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(1175.0f);
	game->Buffer_candlesVertexOffsetData.push_back(300);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle4.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(4 * 64.0f);
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	candle4.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);

	Candle candle5;
	candle5.BufferIndex_candleVertexOffsetData = game->Buffer_candlesVertexOffsetData.size();
	game->Buffer_candlesVertexOffsetData.push_back(750.0f);
	game->Buffer_candlesVertexOffsetData.push_back(150);
	game->Buffer_candlesVertexOffsetData.push_back(0.0f);
	candle5.BufferIndex_candleTextureOffsetData = game->Buffer_candlesTextureOffsetData.size();
	game->Buffer_candlesTextureOffsetData.push_back(5 * 64.0f);
	game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	candle5.BufferIndex_candleStateData = game->Buffer_candlesStateData.size();
	game->Buffer_candlesStateData.push_back(1.0f);
}
