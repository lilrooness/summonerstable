#pragma once
#include "game_structs.h";
#include "sprite.h";

void tickDustBowl(Game* game) {
	resolveScaleAnimations(game->dustBowlSpriteClass, vector<DustBowl>{ game->dustBowl }, game->gameTime);
	
	DustBowl& dustBowl = game->dustBowl;

	float xpos = game->dustBowlSpriteClass.Buffer_vertexOffsetData[game->dustBowl.sprite.BufferIndex_vertexOffsetData];
	float ypos = game->dustBowlSpriteClass.Buffer_vertexOffsetData[game->dustBowl.sprite.BufferIndex_vertexOffsetData + 1];

	if (game->mouseX > xpos && game->mouseX < xpos + DEFUALT_MODEL_SCALE && game->mouseY > ypos && game->mouseY < ypos + DEFUALT_MODEL_SCALE) {
		//Mouse hovering
		if (!dustBowl.mouseHovering) {
			dustBowl.mouseHovering = true;
			IndexReference reference = queueScaleAnimation(game->dustBowlSpriteClass, 0, 1.0f, 1.1f, 5, game->gameTime);
			dustBowl.sprite.scaleAnimationReference = reference;
		}
		//Mouse hovering and down
		else if(game->lmbDown && dustBowl.mouseHovering ){
			cancelScaleAnimation(game->dustBowlSpriteClass, dustBowl.sprite.scaleAnimationReference, dustBowl.sprite);
			dustBowl.mouseHovering = false;
		}

		//Mouse hovering and lifted
		if (game->lmbLifted) {
			game->turnEndedByPlayer = true;
		}
	}
	else if(dustBowl.mouseHovering) {
		dustBowl.mouseHovering = false;
		cancelScaleAnimation(game->dustBowlSpriteClass, dustBowl.sprite.scaleAnimationReference, dustBowl.sprite);
	}
}