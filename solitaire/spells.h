#pragma once
#include "game_structs.h"
#include "animation.h"

void tickSpells(Game* game) {

	resolveScaleAnimations<Spell>(game->spellSpriteClass, game->spells, game->gameTime);

	for (int i = 0; i < game->spells.size(); i++) {

		float spellX = game->spellSpriteClass.Buffer_vertexOffsetData[game->spells[i].sprite.BufferIndex_vertexOffsetData];
		float spellY = game->spellSpriteClass.Buffer_vertexOffsetData[game->spells[i].sprite.BufferIndex_vertexOffsetData + 1];

		float spellSize = DEFUALT_MODEL_SCALE * 0.5;

		if (game->mouseX > spellX && game->mouseX < spellX + spellSize
			&& game->mouseY > spellY && game->mouseY < spellY + spellSize) {

			if (!game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = true;
				game->spells[i].sprite.scaleAnimationReference = queueScaleAnimation(game->spellSpriteClass, i, 0.5, 0.6, 5, game->gameTime);
			}
		}
		else {
			if (game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = false;
				cancelScaleAnimation(game->spellSpriteClass, game->spells[i].sprite.scaleAnimationReference, game->spells[i].sprite);
			}
		}
	}
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