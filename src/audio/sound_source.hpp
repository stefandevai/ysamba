#pragma once

#include <cstdint>

#include "audio/sound_state.hpp"

namespace dl::audio
{
struct SoundSource
{
  uint32_t resource_id;
  bool loop = false;
  uint32_t source = 0;
  SoundState state = SoundState::Initial;
};
}  // namespace dl::audio
