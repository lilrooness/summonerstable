#pragma once

#include <vector>
#include <SDL_mixer.h>
#include <iostream>

struct SoundState {
	std::vector<Mix_Music*> musicTracks;
	std::vector<Mix_Chunk*> sfxTracks;
	int playing{ -1 };
};

bool initSound(SoundState *soundState);
void playMusic(SoundState* soundState, int trackNumber);
void playSoundEffect(SoundState* soundState, int effectNumber);

bool initSound(SoundState *soundState) {
	Mix_Music* backgroundMusic1 = Mix_LoadMUS("sound/crystal_Db.ogg");
	if (backgroundMusic1 == NULL) {
		std::cout << "we didnt load sound/crystal_Db.ogg properly ..." << std::endl;
		std::cout << Mix_GetError() << std::endl;
		return false;
	}
	soundState->musicTracks.push_back(backgroundMusic1);

	Mix_Music* backgroundMusic2 = Mix_LoadMUS("sound/crystal_Gb.ogg");
	if (backgroundMusic2 == NULL) {
		std::cout << "we didnt load sound/crystal_Gb.ogg properly ..." << std::endl;
		std::cout << Mix_GetError() << std::endl;
		return false;
	}
	soundState->musicTracks.push_back(backgroundMusic2);
}

void playMusic(SoundState* soundState, int trackNumber) {
	soundState->playing = trackNumber;
	Mix_PlayMusic(soundState->musicTracks[trackNumber], -1);
}

void playSoundEffect(SoundState* soundState, int effectNumber) {
	Mix_PlayChannel(-1, soundState->sfxTracks[effectNumber], 0);
}
