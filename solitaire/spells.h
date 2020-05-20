#pragma once

#include <map>
#include <algorithm>

#include "game_structs.h"
#include "animation.h"
#include "sprite.h"

void showSpellPopup(Game* game, Spell& spell);
void createNewSpellPopup(Game* game, float x, float y, Spell& spell);
void hideSpellPopup(Game* game, const Spell& spell);
bool canCastSpell(Game* game, const Spell& spell);
void castSpell(Game* game, const Spell& spell);

void tickSpells(Game* game) {

	resolveScaleAnimations<Spell>(game->spellSpriteClass, game->spells, game->gameTime);
	resolveScaleAnimations<SpellPopup>(game->spellPopupSpriteClass, game->spellPopups, game->gameTime);

	for (int i = 0; i < game->spells.size(); i++) {

		float spellX = game->spellSpriteClass.Buffer_vertexOffsetData[game->spells[i].sprite.BufferIndex_vertexOffsetData];
		float spellY = game->spellSpriteClass.Buffer_vertexOffsetData[game->spells[i].sprite.BufferIndex_vertexOffsetData + 1];

		float spellSize = DEFUALT_MODEL_SCALE * 0.5;

		if (game->mouseX > spellX && game->mouseX < spellX + spellSize
			&& game->mouseY > spellY && game->mouseY < spellY + spellSize) {

			if (!game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = true;
				game->spells[i].sprite.scaleAnimationReference = queueScaleAnimation(game->spellSpriteClass, i, 0.5, 0.6, 5, game->gameTime);
				showSpellPopup(game, game->spells[i]);
			}

			if (game->spells[i].mouseHovering && game->lmbDown) {
				castSpell(game, game->spells[i]);
			}
		}
		else {
			if (game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = false;
				cancelScaleAnimation(game->spellSpriteClass, game->spells[i].sprite.scaleAnimationReference, game->spells[i].sprite);
				
				int spellPopupIndex = game->spells[i].spellPopupReference.index;
				cancelScaleAnimation(game->spellPopupSpriteClass, game->spellPopups[spellPopupIndex].sprite.scaleAnimationReference, game->spellPopups[spellPopupIndex].sprite);
				hideSpellPopup(game, game->spells[i]);
			}
		}
	}
}

void hideSpellPopup(Game* game, const Spell& spell) {

	if (spell.spellPopupReference.index < game->spellPopups.size() && spell.spellPopupReference.generation == game->spellPopups[spell.spellPopupReference.index].generation) {
		game->spellPopupSpriteClass.BufferRefreshFlag_vertexOffsetData = true;
		
		Sprite* sprite = &game->spellPopups[spell.spellPopupReference.index].sprite;
		
		game->spellPopupSpriteClass.Buffer_vertexOffsetData[sprite->BufferIndex_vertexOffsetData] -= 2000.0f;
	}
}

void showSpellPopup(Game* game, Spell& spell) {
	
	float spellX = game->spellSpriteClass.Buffer_vertexOffsetData[spell.sprite.BufferIndex_vertexOffsetData];
	float spellY = game->spellSpriteClass.Buffer_vertexOffsetData[spell.sprite.BufferIndex_vertexOffsetData + 1];

	game->spellPopupSpriteClass.BufferRefreshFlag_scaleValueData = true;
	game->spellPopupSpriteClass.BufferRefreshFlag_tintValueData = true;
	game->spellPopupSpriteClass.BufferRefreshFlag_vertexOffsetData = true;
	game->spellPopupSpriteClass.BufferRefreshFlag_textureOffsetData = true;
	
	for (int i = 0; i < game->spellPopups.size(); i++) {
		if (!game->spellPopups[i].showing) {
	
			game->spellPopups[i].showing = true;
			int vertexOffsetBufferIndex = game->spellPopups[i].sprite.BufferIndex_vertexOffsetData;
			game->spellPopupSpriteClass.Buffer_vertexOffsetData[vertexOffsetBufferIndex] = spellX + 100;
			game->spellPopupSpriteClass.Buffer_vertexOffsetData[vertexOffsetBufferIndex + 1] = spellY;
			game->spellPopupSpriteClass.Buffer_scaleValueData[game->spellPopups[i].sprite.BufferIndex_scaleValueData] = 0.0f;
	
			IndexReference popupReference;
			popupReference.generation = ++game->spellPopups[i].generation;
			popupReference.index = i;
			spell.spellPopupReference = popupReference;
	
			queueScaleAnimation(game->spellPopupSpriteClass, popupReference.index, 0, 1.0f, 10, game->gameTime);
	
			return;
		}
	}
	createNewSpellPopup(game, spellX, spellY, spell);
}

void createNewSpellPopup(Game* game, float x, float y, Spell& spell) {
	SpellPopup spellPopup;
	spellPopup.generation = 0;
	spellPopup.showing = true;

	spellPopup.sprite.BufferIndex_scaleValueData = game->spellPopupSpriteClass.Buffer_scaleValueData.size();
	spellPopup.sprite.BufferIndex_textureOffsetData = game->spellPopupSpriteClass.Buffer_textureOffsetData.size();
	spellPopup.sprite.BufferIndex_vertexOffsetData = game->spellPopupSpriteClass.Buffer_vertexOffsetData.size();
	spellPopup.sprite.BufferIndex_tintValueData = game->spellPopupSpriteClass.Buffer_tintValueData.size();


	game->spellPopupSpriteClass.Buffer_vertexOffsetData.push_back(x + 100);
	game->spellPopupSpriteClass.Buffer_vertexOffsetData.push_back(y);
	game->spellPopupSpriteClass.Buffer_vertexOffsetData.push_back(1.0f);

	game->spellPopupSpriteClass.Buffer_tintValueData.push_back(1.0f);
	game->spellPopupSpriteClass.Buffer_tintValueData.push_back(1.0f);
	game->spellPopupSpriteClass.Buffer_tintValueData.push_back(1.0f);

	game->spellPopupSpriteClass.Buffer_textureOffsetData.push_back(spell.type * SPELL_POPUP_SPRITE_WIDTH);
	game->spellPopupSpriteClass.Buffer_textureOffsetData.push_back(SPELL_POPUP_SPRITE_ROW);

	game->spellPopupSpriteClass.Buffer_scaleValueData.push_back(0.0f);


	spell.spellPopupReference.index = game->spellPopups.size();
	spell.spellPopupReference.generation = spellPopup.generation;

	queueScaleAnimation(game->spellPopupSpriteClass, spell.spellPopupReference.index, 0, 1.0f, 10, game->gameTime);

	game->spellPopups.push_back(spellPopup);

}

void summonDemon(Game* game) {

	if (game->summonLevel < 9) {
		game->Buffer_circleTintValueData[game->summonLevel * 3] = 1.0f;
		game->Buffer_circleTintValueData[game->summonLevel * 3 + 1] = 0.0f;
		game->Buffer_circleTintValueData[game->summonLevel * 3 + 2] = 1.0f;

		game->BufferRefreshFlag_circleTintValueData = true;
	}

	game->summonLevel++;
}

void castSpell(Game* game, const Spell& spell) {
	if (canCastSpell(game, spell)) {
		if (spell.type == SpellType::SUMMON_SPELL) {

			//delete cards used for spell
			for (Suit suit : spell.requirements) {
				auto it = std::find_if(game->handCards.begin(), game->handCards.end(), [game, suit](CardReference ref) {
					return ref.cardIndex < game->cards.size() && game->cards[ref.cardIndex].suit == suit;
					});

				if (it != game->handCards.end()) {
					deleteCard(game, *it);
					game->handCards.erase(it);
				}
			}
			summonDemon(game);
		}
	}
}

bool canCastSpell(Game* game, const Spell& spell) {

	std::vector<CardReference> tempHand = game->handCards;

	for (Suit suit : spell.requirements) {
		auto it = std::find_if(tempHand.begin(), tempHand.end(), [game, suit](CardReference ref) {
			return ref.cardIndex < game->cards.size() && game->cards[ref.cardIndex].suit == suit;
		});

		if (it != tempHand.end()) {
			tempHand.erase(it);
		}
		else {
			return false;
		}
	}

	return true;
}
