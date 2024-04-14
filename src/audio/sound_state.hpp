#pragma once

namespace dl::audio
{
enum class SoundState
{
  Initial,
  FadingIn,
  Playing,
  Paused,
  Stopped,
  FadingOut,
};
}
