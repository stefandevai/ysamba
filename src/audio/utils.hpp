#pragma once

struct ALCdevice;

namespace dl::audio
{
void check_alc_error(ALCdevice* device);
void check_al_error();
void check_ogg_error(const int code);
}  // namespace dl::audio
