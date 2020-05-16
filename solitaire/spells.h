#pragma once
#include "game_structs.h"
#include "animation.h"

IndexReference queueSpellScalingAnimation(Game* game, int spellIndex, float toValue, float durationTicks);
void cancelSpellScalingAnimation(Game* game, Spell& spell);

void tickSpells(Game* game) {

	for (int i = 0; i < game->spellScalingAnimations.size(); i++) {
		if (!game->spellScalingAnimations[i].animation.done) {
			game->Buffer_spellsScaleValueData[game->spells[game->spellScalingAnimations[i].spellIndex].BufferIndex_spellScaleValueData] = getCurrentAnimationValue(game->spellScalingAnimations[i].animation, game->gameTime);
			game->BufferRefreshFlag_spellsScaleValueData = true;
		}
	}

	for (int i = 0; i < game->spells.size(); i++) {

		float spellX = game->Buffer_spellsVertexOffsetData[game->spells[i].BufferIndex_spellVertexOffsetData];
		float spellY = game->Buffer_spellsVertexOffsetData[game->spells[i].BufferIndex_spellVertexOffsetData + 1];

		float spellSize = DEFUALT_MODEL_SCALE * 0.5;

		if (game->mouseX > spellX && game->mouseX < spellX + spellSize
			&& game->mouseY > spellY && game->mouseY < spellY + spellSize) {

			if (!game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = true;
				game->spells[i].hoverAnimationReference = queueSpellScalingAnimation(game, i, 0.6f, 5.0f);
			}
		}
		else {
			if (game->spells[i].mouseHovering) {
				game->spells[i].mouseHovering = false;
				cancelSpellScalingAnimation(game, game->spells[i]);
			}
		}
	}
}

void cancelSpellScalingAnimation(Game* game, Spell& spell) {
	if (spell.hoverAnimationReference.index < game->spellScalingAnimations.size() && spell.hoverAnimationReference.generation == game->spellScalingAnimations[spell.hoverAnimationReference.index].generation) {
		game->spellScalingAnimations[spell.hoverAnimationReference.index].animation.done = true;
	}

	game->Buffer_spellsScaleValueData[spell.BufferIndex_spellScaleValueData] = 0.5f;
	game->BufferRefreshFlag_spellsScaleValueData = true;
}

IndexReference queueSpellScalingAnimation(Game* game, int spellIndex, float toValue, float durationTicks) {
	FloatAnimation animation = createAnimation(0.5f, toValue, durationTicks, game->gameTime);
	SpellAnimation spellAnimation;
	spellAnimation.animation = animation;
	spellAnimation.spellIndex = spellIndex;

	for (int i = 0; i < game->spellScalingAnimations.size(); i++) {
		if (game->spellScalingAnimations[i].animation.done) {
			game->spellScalingAnimations[i] = spellAnimation;
			IndexReference indexReference;
			indexReference.generation = ++game->spellScalingAnimations[i].generation;
			indexReference.index = i;
			return indexReference;
		}
	}

	IndexReference indexReference;
	indexReference.generation = 0;
	indexReference.index = game->spellScalingAnimations.size();
	game->spellScalingAnimations.push_back(spellAnimation);

	return indexReference;
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