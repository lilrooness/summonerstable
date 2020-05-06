#pragma once

#include <vector>
#include <GL/GLU.h>
#include <time.h>
#include <stdlib.h>

enum Suit { EYE, BONE, FLESH, BLOOD, HAIR};

struct Card {
	int number;
	Suit suit;
	bool deleted;
	int stackIndex;

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
};

struct Game {
	bool rmbDown{ false}, lmbDown{ false };
	float mouseX{ 0.0f }, mouseY{ 0.0f };
	float lastMouseX{ 0.0f }, lastMouseY{ 0.0f };

	int grabbedCardIndex{ -1 };
	std::vector<Card> cards;
	std::vector<Stack> stacks;

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
//int pickCardFromTableStack(Game* game, float x, float y);
//int cardVertexIndex(int cardId);
bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
//void setCardsZPositionAtStackPosition(Game* game);
//Stack* getCardStack(Game* game, int cardIndex);
//bool cardFitsOnStack(Game *game, Stack& stack, int cardIndex);
//int pickCardFromHand(Game* game, float x, float y);
void addCandles(Game* game);
//void end_turn(Game* game);
//void editCardData(Game* game, int cardIndex, Suit suit, int number);
int createNewCard(Game* game, Suit suit, int number, int stackIndex);
//

int createNewCard(Game* game, Suit suit, int number, int stackIndex) {
	Card card;
	card.deleted = false;
	card.number = number;
	card.suit = suit;
	card.stackIndex = stackIndex;
	card.BufferIndex_cardVertexOffsetData = game->Buffer_cardsVertexOffsetData.size();
	GLfloat x, y, z{0.0f};
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
	game->Buffer_cardsTextureOffsetData.push_back(((GLfloat) suit) * (GLfloat)0.125f);
	game->Buffer_cardsTextureOffsetData.push_back((GLfloat)0.0f);

	card.BufferIndex_numberTextureOffsetData = game->Buffer_numbersTextureOffsetData.size();
	game->Buffer_numbersTextureOffsetData.push_back(((GLfloat)number) * (GLfloat)0.1f);
	game->Buffer_numbersTextureOffsetData.push_back((GLfloat)0.0f);

	int cardIndex = game->cards.size();
	game->cards.push_back(card);

	return cardIndex;
}

void init_game(Game *game) {
	srand(time(NULL));
	//table stacks
	Stack stack1{ 800.0f, 900 };
	Stack stack2{ 400.0f, 600.0f };
	Stack stack3{ 600.0f, 200.0f };
	Stack stack4{ 1000.0f, 200.0f };
	Stack stack5{ 1200.0f, 600.0f };

	game->stacks.push_back(stack1);
	game->stacks.push_back(stack2);
	game->stacks.push_back(stack3);
	game->stacks.push_back(stack4);
	game->stacks.push_back(stack5);

	for (int i = 0; i < game->stacks.size(); i++) {
		Suit cardSuit = static_cast<Suit>( rand() % 5);
		int number = (rand() % 10) + 1;
		createNewCard(game, cardSuit, number, i);
	}

	addCandles(game);

	//int handCards = 3;
	//
	//for (int i = 0; i < handCards; i++) {
	//	Suit cardSuit = static_cast<Suit>(rand() % 5);
	//	Card card{ i, cardSuit};
	//	int cardIndex = addCard(game, card, 150.0f * i + 100, 70.0f, 0.0f);
	//	game->hand.push_back(cardIndex);
	//}

	//setCardsZPositionAtStackPosition(game);
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

	////tring to place card from table
	//if (!game->lmbDown && game->grabbedCard > -1) {
	//	//check for collisions with stacks
	//	GLfloat grabbedX{ game->cardVertexData[cardVertexIndex(game->grabbedCard)] };
	//	GLfloat grabbedY{ game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1]};

	//	bool cardIsDown = false;

	//	if (grabbedY < 50.0f) {
	//		if (game->hand.size() < 5) {
	//			Stack* originalStack = getCardStack(game, game->grabbedCard);
	//			auto stackIt = std::find(originalStack->cardIndexes.begin(), originalStack->cardIndexes.end(), game->grabbedCard);
	//			originalStack->cardIndexes.erase(stackIt);
	//			
	//			auto tableIt = std::find(game->table.begin(), game->table.end(), game->grabbedCard);
	//			game->table.erase(tableIt);

	//			game->hand.push_back(game->grabbedCard);
	//			cardIsDown = true;
	//		}
	//	}

	//	if (!cardIsDown) {
	//		for (Stack& stack : game->stacks) {
	//			if (boxCollision(grabbedX, grabbedY, game->cardWidth, game->cardHeight, stack.x, stack.y, game->cardWidth, game->cardHeight) && cardFitsOnStack(game, stack, game->grabbedCard)) {
	//				game->cardVertexData[cardVertexIndex(game->grabbedCard)] = stack.x;
	//				game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] = stack.y;

	//				Stack* orignal_stack = getCardStack(game, game->grabbedCard);
	//				auto it = std::find(orignal_stack->cardIndexes.begin(), orignal_stack->cardIndexes.end(), game->grabbedCard);
	//				orignal_stack->cardIndexes.erase(it);
	//				stack.cardIndexes.push_back(game->grabbedCard);
	//			}
	//			else {
	//				Stack* orignal_stack = getCardStack(game, game->grabbedCard);
	//				game->cardVertexData[cardVertexIndex(game->grabbedCard)] = orignal_stack->x;
	//				game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] = orignal_stack->y;
	//			}
	//		}
	//	}

	//	setCardsZPositionAtStackPosition(game);
	//	game->cardGrabbedData[game->grabbedCard] = (GLboolean)false;
	//	game->grabbedCard = -1;
	//	game->cardVertexDataUpdated = true;
	//}
	////trying to place card from hand
	//else if (!game->lmbDown && game->grabbedHandCard > -1) {
	//	//check for collisions with stacks
	//	GLfloat grabbedX{ game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] };
	//	GLfloat grabbedY{ game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] };

	//	for (Stack& stack : game->stacks) {
	//		if (boxCollision(grabbedX, grabbedY, game->cardWidth, game->cardHeight, stack.x, stack.y, game->cardWidth, game->cardHeight) && cardFitsOnStack(game, stack, game->grabbedHandCard)) {
	//			game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] = stack.x;
	//			game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] = stack.y;
	//			game->cardVertexDataUpdated = true;
	//			game->table.push_back(game->grabbedHandCard);
	//			stack.cardIndexes.push_back(game->grabbedHandCard);

	//			auto handIt = std::find(game->hand.begin(), game->hand.end(), game->grabbedHandCard);
	//			game->hand.erase(handIt);
	//		}
	//	}

	//	setCardsZPositionAtStackPosition(game);
	//	game->cardGrabbedData[game->grabbedHandCard] = (GLboolean)false;
	//	game->grabbedHandCard = -1;
	//	game->cardVertexDataUpdated = true;

	//}

	//if (game->grabbedCard > -1) {
	//	float mouseMovementX = game->mouseX - game->lastMouseX;
	//	float mouseMovementY = game->mouseY - game->lastMouseY;
	//	game->cardVertexData[cardVertexIndex(game->grabbedCard)] += mouseMovementX;
	//	game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] += mouseMovementY;
	//	game->cardVertexData[cardVertexIndex(game->grabbedCard) + 2] = 10.0f;
	//	game->cardVertexDataUpdated = true;
	//}
	//else if (game->grabbedHandCard > -1) {
	//	float mouseMovementX = game->mouseX - game->lastMouseX;
	//	float mouseMovementY = game->mouseY - game->lastMouseY;
	//	game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] += mouseMovementX;
	//	game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] += mouseMovementY;
	//	game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 2] = 10.0f;
	//	game->cardVertexDataUpdated = true;
	//}
	//else if (game->lmbDown) {
	//	int pickedCard = pickCardFromTableStack(game, game->mouseX, game->mouseY);
	//	if (pickedCard > -1) {
	//		game->grabbedCard = pickedCard;
	//		game->cardGrabbedData[pickedCard] = (GLboolean)true;
	//	}
	//	else {
	//		pickedCard = pickCardFromHand(game, game->mouseX, game->mouseY);
	//		if (pickedCard > -1) {
	//			game->grabbedHandCard = pickedCard;
	//			game->cardGrabbedData[pickedCard] = (GLboolean)true;
	//		}
	//	}
	//}
}
//
//int pickCardFromHand(Game* game, float x, float y) {
//
//	for (int i : game->hand) {
//		GLfloat posx = game->cardVertexData[cardVertexIndex(i)];
//		GLfloat posy = game->cardVertexData[cardVertexIndex(i) + 1];
//
//		if (x > posx && x < posx + game->cardWidth && y > posy && y < posy + game->cardHeight) {
//			return i;
//		}
//	}
//
//	return -1;
//}
//
//int pickCardFromTableStack(Game* game, float x, float y) {
//	std::vector<int> collidedCardIndexes;
//
//	for (int i : game->table) {
//		GLfloat posx = game->cardVertexData[cardVertexIndex(i)];
//		GLfloat posy = game->cardVertexData[cardVertexIndex(i) + 1];
//
//		if (x > posx && x < posx + game->cardWidth && y > posy && y < posy + game->cardHeight) {
//			collidedCardIndexes.push_back(i);
//		}
//	}
//
//	if (collidedCardIndexes.size() == 0) {
//		return -1;
//	}
//	else {
//		Stack* stack = getCardStack(game, collidedCardIndexes[0]);
//		return stack->cardIndexes.back();
//	}
//}
//
//int cardVertexIndex(int cardId) {
//	return cardId * 3;
//}
//
//bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
//	float x1Center = x1 + w1 / 2;
//	float y1Center = y1 + h1 / 2;
//
//	float x2Center = x2 + w2 / 2;
//	float y2Center = y2 + h2 / 2;
//
//	return (abs(x1Center - x2Center) < ((w1 + w2) / 2) && abs(y1Center - y2Center) < ((h1 + h2) / 2));
//}
//
//void setCardsZPositionAtStackPosition(Game* game) {
//	GLfloat zIncrement = 0.1f;
//	for (Stack stack : game->stacks) {
//		for (int i = 0; i < stack.cardIndexes.size(); i++) {
//			int cardIndex = stack.cardIndexes[i];
//			game->cardVertexData[cardVertexIndex(cardIndex) + 2] = (GLfloat)i * (GLfloat)zIncrement;
//			game->cardVertexDataUpdated = true;
//		}
//	}
//}
//
//Stack* getCardStack(Game* game, int cardIndex) {
//	for (Stack& stack : game->stacks) {
//		if (std::find(stack.cardIndexes.begin(), stack.cardIndexes.end(), cardIndex) != stack.cardIndexes.end()) {
//			return &stack;
//		}
//	}
//
//	return nullptr;
//}
//
//bool cardFitsOnStack(Game *game, Stack& stack, int cardIndex) {
//
//	if (stack.cardIndexes.size() == 0) {
//		return true;
//	}
//
//	int topCardIndex = stack.cardIndexes.back();
//	if (game->cards[topCardIndex].suit == game->cards[cardIndex].suit) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
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

	//for (int i = 0; i < 5; i++) {
	//	//game->Buffer_candlesTextureOffsetData.push_back(i * 64.0f);
	//	//game->Buffer_candlesTextureOffsetData.push_back(0.0f);
	//	//
	//	game->Buffer_candlesStateData.push_back(1.0f);
	//}
}
//
//void end_turn(Game* game) {
//	int candleOutCount = 0;
//	for (Stack& stack : game->stacks) {
//		if (stack.cardIndexes.size() == 1) {
//			game->candles[candleOutCount] = 0.0f;
//			game->candleStateChanged = true;
//			candleOutCount += 1;
//		}
//	}
//
//	int newCardsCreated = 0;
//
//	//clear all the stacks
//	for (Stack& stack : game->stacks) {
//		stack.cardIndexes.clear();
//	}
//
//	//reuse existing cards left on the table and distribute in to the different stacks
//	for (int cardIndex : game->table) {
//		Suit suit = static_cast<Suit>(rand() % 5);
//		int number = (rand() % 10) + 1;
//		game->cards[cardIndex].number = number;
//		game->cards[cardIndex].suit = suit;
//
//		editCardData(game, cardIndex, suit, number);
//		game->stacks[newCardsCreated].cardIndexes.push_back(cardIndex);
//		game->cardVertexData[cardVertexIndex(cardIndex)] = game->stacks[newCardsCreated].x;
//		game->cardVertexData[cardVertexIndex(cardIndex) + 1] = game->stacks[newCardsCreated].y;
//		newCardsCreated++;
//	}
//
//	//if needed, add more cards to the table
//	int cardsToAdd = 5 - newCardsCreated;
//	for (int i = 0; i < cardsToAdd; i++) {
//		Stack* stack = &game->stacks[newCardsCreated];
//		Card card;
//		card.suit = static_cast<Suit>(rand() % 5);
//		card.number = (rand() % 10) + 1;
//		int newCardIndex = addCard(game, card, stack->x, stack->y, 0.0f);
//		stack->cardIndexes.push_back(newCardIndex);
//		game->table.push_back(newCardIndex);
//		newCardsCreated++;
//	}
//	setCardsZPositionAtStackPosition(game);
//}
//
//void editCardData(Game* game, int cardIndex, Suit suit, int number) {
//	game->cards[cardIndex].suit = suit;
//	game->cards[cardIndex].number =  number;
//	game->textureOffsetData[cardIndex * 2] = (GLfloat)suit * (GLfloat)0.125f;
//	game->cardGrabbedData.push_back((GLboolean)false);
//	game->cardTextureTransformDataUpdated = true;
//}
