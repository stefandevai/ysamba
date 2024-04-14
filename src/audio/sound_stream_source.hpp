#pragma once

#include <cstdint>

#include "audio/sound_state.hpp"

namespace dl::audio
{
class SoundStreamBuffer;

struct SoundStreamSource
{
  uint32_t resource_id;
  bool loop = false;
  bool fade = false;
  float gain = 1.0f;
  uint32_t source = 0;
  SoundState state = SoundState::Initial;
  SoundStreamBuffer* buffer = nullptr;

  SoundStreamSource(uint32_t resource_id, bool loop, bool fade) : resource_id(resource_id), loop(loop), fade(fade) {}
};
}  // namespace dl::audio
