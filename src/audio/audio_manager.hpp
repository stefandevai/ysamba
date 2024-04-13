#pragma once

#include <memory>

#include "audio/sound_stream_buffer.hpp"
#include "audio/sound_buffer.hpp"

struct ALCdevice;
struct ALCcontext;

namespace dl
{
class AssetManager;
}

namespace dl::audio
{
class AudioManager
{
 public:
  AudioManager(AssetManager& asset_manager);
  ~AudioManager();

  void update();

 private:
  AssetManager& m_asset_manager;
  ALCdevice* m_device = nullptr;
  ALCcontext* m_context = nullptr;
  std::unique_ptr<SoundStreamBuffer> m_stream = nullptr;
  std::unique_ptr<SoundBuffer> m_sound = nullptr;
};
}  // namespace dl::audio
