#pragma once

#include <memory>
#include "audio/ogg_data.hpp"

struct ALCdevice;
struct ALCcontext;

namespace dl::audio
{
class AudioManager
{
public:
  AudioManager();
  ~AudioManager();

  void test();
  void update();

private:
  ALCdevice* m_device = nullptr;
  ALCcontext* m_context = nullptr;
  std::unique_ptr<OggData> m_stream = nullptr;
};
}
