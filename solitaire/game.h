#pragma once

#include <vector>
#include <GL/GLU.h>
#include <time.h>
#include <stdlib.h>

enum Suit { EYE, BONE, BLOOD, SKIN, HAIR};

struct Card {
	int number;
	Suit suit;
};

struct Stack {
	float x, y;
	std::vector<int> cardIndexes;
};

struct Game {
	std::vector<GLfloat> cardVertexData;
	std::vector<GLfloat> textureOffsetData;
	std::vector<GLboolean> cardGrabbedData;

	std::vector<GLfloat> numbersTextureOffsetData;

	std::vector<Card> cards;
	std::vector<Stack> stacks;
	std::vector<Stack> handStacks;
	std::vector<int> hand;
	std::vector<int> table;
	
	float cardWidth{ 0.25f * 512.0f }, cardHeight{ 0.5f * 512.0f };
	bool cardVertexDataUpdated{ false };
	bool rmbDown{ false}, lmbDown{ false };
	float mouseX{ 0.0f }, mouseY{ 0.0f };
	float lastMouseX{ 0.0f }, lastMouseY{ 0.0f };
	bool up{ false }, right{ false }, down{ false }, left{ false };
	float speed {2.0f};
	int grabbedCard { -1 };
	int grabbedHandCard{ -1 };
};

void init_game(Game *game);
void tick(Game *game, float mouseX, float mouseY, float dt);
int pickCardFromTableStack(Game* game, float x, float y);
int cardVertexIndex(int cardId);
bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void setCardsZPositionAtStackPosition(Game* game);
Stack* getCardStack(Game* game, int cardIndex);
bool cardFitsOnStack(Game *game, Stack& stack, int cardIndex);
int addCard(Game* game, Card card, GLfloat x, GLfloat y, GLfloat z);
int pickCardFromHand(Game* game, float x, float y);

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

	int cards = 5;
	//init table cards
	for (int i = 0; i < cards; i++) {

		Suit cardSuit = static_cast<Suit>( rand() % 5);
		int number = (rand() % 10) + 1;
		Card card{ i, cardSuit };
		int cardIndex = addCard(game, card, game->stacks[i].x, game->stacks[i].y, 0.0f);
		game->stacks[i].cardIndexes.push_back(cardIndex);
		game->table.push_back(cardIndex);
	}

	int handCards = 3;
	
	for (int i = 0; i < handCards; i++) {
		Card card{ i, Suit::BONE };
		int cardIndex = addCard(game, card, 300.0f * i + 300, 300.0f, 0.0f);
		game->hand.push_back(cardIndex);
	}

	setCardsZPositionAtStackPosition(game);
}

int addCard(Game *game, Card card, GLfloat x, GLfloat y, GLfloat z) {
	game->cards.push_back(card);
	int cardIndex = game->cards.size() - 1;
	game->cardVertexData.push_back(x);
	game->cardVertexData.push_back(y);
	game->cardVertexData.push_back(z);

	game->textureOffsetData.push_back((GLfloat)card.suit * (GLfloat)0.125f);
	game->textureOffsetData.push_back((GLfloat)0.0f);

	game->numbersTextureOffsetData.push_back((GLfloat)(card.number) * 0.1f);
	game->numbersTextureOffsetData.push_back((GLfloat)0.0f);

	game->cardGrabbedData.push_back((GLboolean)false);

	return cardIndex;
}

void tick(Game *game, float mouseX, float mouseY, float dt) {

	game->lastMouseX = game->mouseX;
	game->lastMouseY = game->mouseY;
	game->mouseX = mouseX;
	game->mouseY = mouseY;

	game->cardVertexDataUpdated = false;

	//tring to place card from table
	if (!game->lmbDown && game->grabbedCard > -1) {
		//check for collisions with stacks
		GLfloat grabbedX{ game->cardVertexData[cardVertexIndex(game->grabbedCard)] };
		GLfloat grabbedY{ game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1]};

		bool cardIsDown = false;

		if (grabbedY < 50.0f) {
			if (game->hand.size() < 5) {
				Stack* originalStack = getCardStack(game, game->grabbedCard);
				auto stackIt = std::find(originalStack->cardIndexes.begin(), originalStack->cardIndexes.end(), game->grabbedCard);
				originalStack->cardIndexes.erase(stackIt);
				
				auto tableIt = std::find(game->table.begin(), game->table.end(), game->grabbedCard);
				game->table.erase(tableIt);

				game->hand.push_back(game->grabbedCard);
				cardIsDown = true;
			}
		}

		if (!cardIsDown) {
			for (Stack& stack : game->stacks) {
				if (boxCollision(grabbedX, grabbedY, game->cardWidth, game->cardHeight, stack.x, stack.y, game->cardWidth, game->cardHeight) && cardFitsOnStack(game, stack, game->grabbedCard)) {
					game->cardVertexData[cardVertexIndex(game->grabbedCard)] = stack.x;
					game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] = stack.y;

					Stack* orignal_stack = getCardStack(game, game->grabbedCard);
					auto it = std::find(orignal_stack->cardIndexes.begin(), orignal_stack->cardIndexes.end(), game->grabbedCard);
					orignal_stack->cardIndexes.erase(it);
					stack.cardIndexes.push_back(game->grabbedCard);
				}
				else {
					Stack* orignal_stack = getCardStack(game, game->grabbedCard);
					game->cardVertexData[cardVertexIndex(game->grabbedCard)] = orignal_stack->x;
					game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] = orignal_stack->y;
				}
			}
		}

		setCardsZPositionAtStackPosition(game);
		game->cardGrabbedData[game->grabbedCard] = (GLboolean)false;
		game->grabbedCard = -1;
		game->cardVertexDataUpdated = true;
	}
	//trying to place card from hand
	else if (!game->lmbDown && game->grabbedHandCard > -1) {
		//check for collisions with stacks
		GLfloat grabbedX{ game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] };
		GLfloat grabbedY{ game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] };

		for (Stack& stack : game->stacks) {
			if (boxCollision(grabbedX, grabbedY, game->cardWidth, game->cardHeight, stack.x, stack.y, game->cardWidth, game->cardHeight) && cardFitsOnStack(game, stack, game->grabbedHandCard)) {
				game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] = stack.x;
				game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] = stack.y;
				game->cardVertexDataUpdated = true;
				game->table.push_back(game->grabbedHandCard);
				stack.cardIndexes.push_back(game->grabbedHandCard);

				auto handIt = std::find(game->hand.begin(), game->hand.end(), game->grabbedHandCard);
				game->hand.erase(handIt);
			}
		}

		setCardsZPositionAtStackPosition(game);
		game->cardGrabbedData[game->grabbedHandCard] = (GLboolean)false;
		game->grabbedHandCard = -1;
		game->cardVertexDataUpdated = true;

	}

	if (game->grabbedCard > -1) {
		float mouseMovementX = game->mouseX - game->lastMouseX;
		float mouseMovementY = game->mouseY - game->lastMouseY;
		game->cardVertexData[cardVertexIndex(game->grabbedCard)] += mouseMovementX;
		game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1] += mouseMovementY;
		game->cardVertexData[cardVertexIndex(game->grabbedCard) + 2] = 10.0f;
		game->cardVertexDataUpdated = true;
	}
	else if (game->grabbedHandCard > -1) {
		float mouseMovementX = game->mouseX - game->lastMouseX;
		float mouseMovementY = game->mouseY - game->lastMouseY;
		game->cardVertexData[cardVertexIndex(game->grabbedHandCard)] += mouseMovementX;
		game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 1] += mouseMovementY;
		game->cardVertexData[cardVertexIndex(game->grabbedHandCard) + 2] = 10.0f;
		game->cardVertexDataUpdated = true;
	}
	else if (game->lmbDown) {
		int pickedCard = pickCardFromTableStack(game, game->mouseX, game->mouseY);
		if (pickedCard > -1) {
			game->grabbedCard = pickedCard;
			game->cardGrabbedData[pickedCard] = (GLboolean)true;
		}
		else {
			pickedCard = pickCardFromHand(game, game->mouseX, game->mouseY);
			if (pickedCard > -1) {
				game->grabbedHandCard = pickedCard;
				game->cardGrabbedData[pickedCard] = (GLboolean)true;
			}
		}
	}
}

int pickCardFromHand(Game* game, float x, float y) {

	for (int i : game->hand) {
		GLfloat posx = game->cardVertexData[cardVertexIndex(i)];
		GLfloat posy = game->cardVertexData[cardVertexIndex(i) + 1];

		if (x > posx && x < posx + game->cardWidth && y > posy && y < posy + game->cardHeight) {
			return i;
		}
	}

	return -1;
}

int pickCardFromTableStack(Game* game, float x, float y) {
	std::vector<int> collidedCardIndexes;

	for (int i : game->table) {
		GLfloat posx = game->cardVertexData[cardVertexIndex(i)];
		GLfloat posy = game->cardVertexData[cardVertexIndex(i) + 1];

		if (x > posx && x < posx + game->cardWidth && y > posy && y < posy + game->cardHeight) {
			collidedCardIndexes.push_back(i);
		}
	}

	if (collidedCardIndexes.size() == 0) {
		return -1;
	}
	else {
		Stack* stack = getCardStack(game, collidedCardIndexes[0]);
		return stack->cardIndexes.back();
	}
}

int cardVertexIndex(int cardId) {
	return cardId * 3;
}

bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	float x1Center = x1 + w1 / 2;
	float y1Center = y1 + h1 / 2;

	float x2Center = x2 + w2 / 2;
	float y2Center = y2 + h2 / 2;

	return (abs(x1Center - x2Center) < ((w1 + w2) / 2) && abs(y1Center - y2Center) < ((h1 + h2) / 2));
}

void setCardsZPositionAtStackPosition(Game* game) {
	GLfloat zIncrement = 0.1f;
	for (Stack stack : game->stacks) {
		for (int i = 0; i < stack.cardIndexes.size(); i++) {
			int cardIndex = stack.cardIndexes[i];
			game->cardVertexData[cardVertexIndex(cardIndex) + 2] = (GLfloat)i * (GLfloat)zIncrement;
		}
	}
}

Stack* getCardStack(Game* game, int cardIndex) {
	for (Stack& stack : game->stacks) {
		if (std::find(stack.cardIndexes.begin(), stack.cardIndexes.end(), cardIndex) != stack.cardIndexes.end()) {
			return &stack;
		}
	}

	return nullptr;
}

bool cardFitsOnStack(Game *game, Stack& stack, int cardIndex) {

	if (stack.cardIndexes.size() == 0) {
		return true;
	}

	int topCardIndex = stack.cardIndexes.back();
	if (game->cards[topCardIndex].suit == game->cards[cardIndex].suit) {
		return true;
	}
	else {
		return false;
	}
}