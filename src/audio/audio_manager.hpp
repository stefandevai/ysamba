#pragma once

#include <memory>

#include "audio/sound_buffer.hpp"
#include "audio/sound_source.hpp"
#include "audio/sound_stream_buffer.hpp"
#include "audio/sound_stream_source.hpp"

struct ALCdevice;
struct ALCcontext;

namespace dl
{
class AssetManager;
struct Vector3i;
}  // namespace dl

namespace dl::audio
{
class AudioManager
{
 public:
  AudioManager(AssetManager& asset_manager);
  ~AudioManager();

  SoundSource* sound_effect(uint32_t resource_id, bool loop = false, bool fade = false);
  SoundStreamSource* music(uint32_t resource_id, bool loop = false, bool fade = false);
  void update();

  // void play(SoundSource* source);
  void pause(SoundSource* source);
  void resume(SoundSource* source);
  void stop(SoundSource*& source);
  void destroy(SoundSource*& source);

  void pause(SoundStreamSource* source);
  void resume(SoundStreamSource* source);
  void stop(SoundStreamSource*& source);

 private:
  AssetManager& m_asset_manager;
  ALCdevice* m_device = nullptr;
  ALCcontext* m_context = nullptr;
  std::vector<std::unique_ptr<SoundSource>> m_sound_sources{};
  std::vector<std::unique_ptr<SoundStreamSource>> m_sound_stream_sources{};
};
}  // namespace dl::audio
