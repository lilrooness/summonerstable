#pragma once

#include <vector>
#include <GL/GLU.h>

enum Suit {BONE, EYE, BLOOD, SKIN, HAIR};

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
};

void init_game(Game *game);
void tick(Game *game, float mouseX, float mouseY, float dt);
int pickCardFromStack(Game* game, float x, float y);
int cardVertexIndex(int cardId);
bool boxCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void setCardsZPositionAtStackPosition(Game* game);
Stack* getCardStack(Game* game, int cardIndex);
bool cardFitsOnStack(Game *game, Stack& stack, int cardIndex);

void init_game(Game *game) {

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
		game->stacks[i].cardIndexes.push_back(i);
		game->cardVertexData.push_back((GLfloat)game->stacks[i].x); //x coord
		game->cardVertexData.push_back((GLfloat)game->stacks[i].y); //y coord
		game->cardVertexData.push_back((GLfloat)0.0f); //z coord

		game->textureOffsetData.push_back((GLfloat)(i % 2) * (GLfloat)0.125f);
		game->textureOffsetData.push_back((GLfloat)0.0f);

		game->cardGrabbedData.push_back((GLboolean)false);
		Suit cardSuit;
		if (i % 2 == 0) {
			cardSuit = Suit::EYE;
		}
		else {
			cardSuit = Suit::BONE;
		}
		Card card{ i, cardSuit};
		game->cards.push_back(card);

		game->numbersTextureOffsetData.push_back((GLfloat)(card.number) * 0.1f);
		game->numbersTextureOffsetData.push_back((GLfloat)0.0f);

		game->table.push_back(i);
	}
	
	int handSize = 3;
	//init hand cards
	for (int i = 0; i < handSize; i++) {
		Card card;
		card.number = i;
		Suit cardSuit;
		if (i % 2 == 0) {
			cardSuit = Suit::BONE;
		}
		else {
			cardSuit = Suit::EYE;
		}
		card.suit = cardSuit;
		game->cards.push_back(card);
		game->hand.push_back(game->cards.size() - 1);
		
		game->cardVertexData.push_back((GLfloat) i * 300.0f); // x
		game->cardVertexData.push_back((GLfloat) 100.0f);     // y
		game->cardVertexData.push_back((GLfloat) 0.0f);       // z
	}

	setCardsZPositionAtStackPosition(game);
}

void tick(Game *game, float mouseX, float mouseY, float dt) {

	game->lastMouseX = game->mouseX;
	game->lastMouseY = game->mouseY;
	game->mouseX = mouseX;
	game->mouseY = mouseY;

	game->cardVertexDataUpdated = false;
	if (!game->lmbDown && game->grabbedCard > -1) {
		game->cardGrabbedData[game->grabbedCard] = (GLboolean)false;
		game->cardVertexData[cardVertexIndex(game->grabbedCard) + 2] = 0.0f;
		
		//check for collisions with stacks
		GLfloat grabbedX{ game->cardVertexData[cardVertexIndex(game->grabbedCard)] };
		GLfloat grabbedY{ game->cardVertexData[cardVertexIndex(game->grabbedCard) + 1]};
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

		setCardsZPositionAtStackPosition(game);
		game->grabbedCard = -1;
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
	else {
		int pickedCard = pickCardFromStack(game, game->mouseX, game->mouseY);
		if (pickedCard > -1 && game->lmbDown) {
			game->grabbedCard = pickedCard;
			game->cardGrabbedData[pickedCard] = (GLboolean)true;
		}
	}
}

int pickCardFromStack(Game* game, float x, float y) {
	std::vector<int> collidedCardIndexes;

	for (int i = 0; i < game->table.size(); i ++) {
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