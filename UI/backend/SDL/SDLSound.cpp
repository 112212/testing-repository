#include <SDL2/SDL.h>
#include <iostream>

#include "SDLSound.hpp"

namespace ng {
SDLSound::~SDLSound() {
	SDL_FreeWAV(buffer);
}


void SDLSound::FreeCache() {
	
}

void SDLSound::Free() {
	
}

int SDLSound::GetSoundSize() {
	return length;
}

const void* SDLSound::GetSound(uint32_t offset, uint32_t requested_size) {
	return buffer+offset;
}

uint32_t SDLSound::GetSoundId() {
	
}

int SDLSound::GetSoundChannels() {
	return 2;
}

int SDLSound::GetSoundFrames() {
	return length/(sizeof(int16_t) * GetSoundChannels());
}


SDLSound::SDLSound(std::string filename) {
	SDL_AudioSpec wav_spec;
	
	wav_spec.userdata = NULL;
	// wav_spec.freq = 44100;
	wav_spec.freq = 48000;
	wav_spec.format = AUDIO_S16;
	wav_spec.channels = 2;
	wav_spec.samples = 4096;
	
	if (SDL_LoadWAV(filename.c_str(), &wav_spec, &buffer, &length) == NULL) {
		fprintf(stderr, "%s\n", SDL_GetError());
	}
}

Sound* SDLSound::LoadSound(File file) {
	return new SDLSound( file.name );
}

}
