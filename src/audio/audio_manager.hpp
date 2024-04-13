#pragma once

#include <memory>

#include "audio/ogg_data.hpp"
#include "audio/ogg_stream.hpp"
#include "audio/sound.hpp"

struct ALCdevice;
struct ALCcontext;

namespace dl::audio
{
class AudioManager
{
 public:
  AudioManager();
  ~AudioManager();

  void update();

 private:
  ALCdevice* m_device = nullptr;
  ALCcontext* m_context = nullptr;
  std::unique_ptr<OggStream> m_stream = nullptr;
  std::unique_ptr<Sound> m_sound = nullptr;
};
}  // namespace dl::audio
