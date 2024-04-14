#pragma once

#include <AL/al.h>

#include "audio/sound_state.hpp"

struct ALCdevice;

namespace dl::utils
{
void check_alc_error(ALCdevice* device);
void check_al_error();
void check_ogg_error(const int code);
audio::SoundState al_state_to_sound_state(ALenum state);
}  // namespace dl::utils
