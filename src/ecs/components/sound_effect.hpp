#pragma once

#include <cstdint>

namespace dl::audio
{
struct SoundSource;
}

namespace dl
{
struct SoundEffect
{
  uint32_t id = 0;
  bool loop = false;
  bool fade = false;
  audio::SoundSource* source = nullptr;
};
}  // namespace dl
