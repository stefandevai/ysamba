#include "./audio_manager.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <spdlog/spdlog.h>

#include "audio/utils.hpp"

namespace dl::audio
{
AudioManager::AudioManager()
{
  ALCdevice* m_device = alcOpenDevice(nullptr);

  if (m_device == nullptr)
  {
    spdlog::critical("Failed to open OpenAL device");
    return;
  }

  m_context = alcCreateContext(m_device, nullptr);
  check_alc_error(m_device);

  if (m_context == nullptr)
  {
    spdlog::critical("Error loading ALC context");
    alcCloseDevice(m_device);
    return;
  }

  alcMakeContextCurrent(m_context);
  check_alc_error(m_device);

  // m_stream = std::make_unique<OggStream>("data/audio/theme.ogg");
  // m_stream->play(false);
  m_sound = std::make_unique<Sound>("data/audio/theme.ogg");
  m_sound->play(false);
}

AudioManager::~AudioManager()
{
  m_sound->destroy();
  m_stream->destroy();
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

void AudioManager::update()
{
  // m_stream->update();
  m_sound->update();
}
}  // namespace dl::audio
