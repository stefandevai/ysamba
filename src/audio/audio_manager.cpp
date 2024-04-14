#include "./audio_manager.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <spdlog/spdlog.h>

#include "audio/utils.hpp"
#include "core/asset_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl::audio
{
AudioManager::AudioManager(AssetManager& asset_manager) : m_asset_manager(asset_manager)
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

  // m_stream = std::make_unique<SoundStreamBuffer>("data/audio/theme.ogg");
  // m_stream->load();
  // m_stream->play();
  // m_sound = std::make_unique<SoundBuffer>("data/audio/click.ogg");
  // m_sound->load();
  // m_sound->play(true);
}

AudioManager::~AudioManager()
{
  for (auto& source : m_sound_sources)
  {
    alSourceStop(source.source);
    alDeleteSources(1, &source.source);
  }

  for (auto& source : m_sound_stream_sources)
  {
    source.buffer->stop(source.source);
    alSourceStop(source.source);
    alDeleteSources(1, &source.source);
  }
  // m_sound->destroy();
  // m_stream->destroy();
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

SoundSource& AudioManager::sound_effect(const uint32_t resource_id, const bool loop)
{
  SoundSource source{resource_id, loop};

  const auto& buffer = m_asset_manager.get<SoundBuffer>(source.resource_id);

  alGenSources(1, &source.source);
  check_al_error();
  alSourcef(source.source, AL_PITCH, 1.0f);
  alSourcef(source.source, AL_GAIN, 1.0f);
  alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcei(source.source, AL_BUFFER, buffer->id);
  alSourcei(source.source, AL_LOOPING, source.loop);
  alSourcePlay(source.source);
  check_al_error();

  source.state = SoundState::Playing;

  m_sound_sources.push_back(std::move(source));

  return m_sound_sources.back();
}

SoundStreamSource& AudioManager::music(const uint32_t resource_id, const bool loop)
{
  SoundStreamSource source{resource_id, loop};

  const auto& buffer = m_asset_manager.get<SoundStreamBuffer>(source.resource_id);

  alGenSources(1, &source.source);
  check_al_error();
  alSourcef(source.source, AL_PITCH, 1.0f);
  alSourcef(source.source, AL_GAIN, 1.0f);
  alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcei(source.source, AL_LOOPING, source.loop);
  alSourcePlay(source.source);
  check_al_error();

  source.buffer = buffer;
  buffer->play(source.source, source.loop);
  source.state = SoundState::Playing;

  m_sound_stream_sources.push_back(std::move(source));

  return m_sound_stream_sources.back();
}

void AudioManager::play(SoundSource& source)
{
  if (source.state == SoundState::Initial)
  {
    const auto& buffer = m_asset_manager.get<SoundBuffer>(source.resource_id);

    alGenSources(1, &source.source);
    check_al_error();
    alSourcef(source.source, AL_PITCH, 1.0f);
    alSourcef(source.source, AL_GAIN, 1.0f);
    alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
    alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(source.source, AL_BUFFER, buffer->id);
  }

  ALenum state;
  alGetSourcei(source.source, AL_SOURCE_STATE, &state);

  if (state == AL_PLAYING)
  {
    return;
  }

  source.state = SoundState::Playing;
  alSourcei(source.source, AL_LOOPING, source.loop);
  alSourcePlay(source.source);
}

void AudioManager::pause(SoundSource& source)
{
  alSourcePause(source.source);
}

void AudioManager::resume(SoundSource& source)
{
  alSourcePlay(source.source);
}

void AudioManager::stop(SoundSource& source)
{
  alSourceStop(source.source);
}

void AudioManager::destroy(SoundSource& source)
{
  alSourceStop(source.source);
  alDeleteSources(1, &source.source);
  source.source = 0;
  source.state = SoundState::Initial;
}

void AudioManager::update()
{
  for (auto& source : m_sound_stream_sources)
  {
    source.buffer->update(source.source);
  }
  // m_stream->update();
}
}  // namespace dl::audio
