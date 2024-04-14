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
  utils::check_alc_error(m_device);

  if (m_context == nullptr)
  {
    spdlog::critical("Error loading ALC context");
    alcCloseDevice(m_device);
    return;
  }

  alcMakeContextCurrent(m_context);
  utils::check_alc_error(m_device);
}

AudioManager::~AudioManager()
{
  // Delete existing sources
  for (auto& source : m_sound_sources)
  {
    alSourceStop(source.source);
    alDeleteSources(1, &source.source);
  }

  for (auto& source : m_sound_stream_sources)
  {
    source->buffer->stop(source->source);
    alDeleteSources(1, &source->source);
  }

  // Destroy actual audio buffers before closing the device
  const auto& asset_ids = m_asset_manager.get_ids();

  for (const auto& handle : asset_ids)
  {
    if (handle.second == AssetType::Music)
    {
      auto buffer = m_asset_manager.get<SoundStreamBuffer>(handle.first);
      buffer->destroy();
    }
    else if (handle.second == AssetType::SoundEffect)
    {
      auto buffer = m_asset_manager.get<SoundBuffer>(handle.first);
      buffer->destroy();
    }
  }

  utils::check_al_error();

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

SoundSource& AudioManager::sound_effect(const uint32_t resource_id, const bool loop)
{
  SoundSource source{resource_id, loop};

  const auto& buffer = m_asset_manager.get<SoundBuffer>(source.resource_id);

  alGenSources(1, &source.source);
  utils::check_al_error();
  alSourcef(source.source, AL_PITCH, 1.0f);
  alSourcef(source.source, AL_GAIN, 1.0f);
  alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcei(source.source, AL_BUFFER, buffer->id);
  alSourcei(source.source, AL_LOOPING, source.loop);
  alSourcePlay(source.source);
  utils::check_al_error();

  source.buffer = buffer;
  source.state = SoundState::Playing;

  m_sound_sources.push_back(std::move(source));

  return m_sound_sources.back();
}

SoundStreamSource* AudioManager::music(const uint32_t resource_id, const bool loop, const bool fade_in)
{
  auto source_ptr = std::make_unique<SoundStreamSource>(resource_id, loop, fade_in);
  auto& source = *source_ptr;

  const auto& buffer = m_asset_manager.get<SoundStreamBuffer>(source.resource_id);

  alGenSources(1, &source.source);
  utils::check_al_error();
  alSourcef(source.source, AL_PITCH, 1.0f);

  alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcei(source.source, AL_LOOPING, false);

  if (source.fade_in)
  {
    alSourcef(source.source, AL_GAIN, 0.0f);
    source.gain = 0.0f;
    source.state = SoundState::FadingIn;
  }
  else
  {
    alSourcef(source.source, AL_GAIN, 1.0f);
    source.gain = 1.0f;
    source.state = SoundState::Playing;
  }

  alSourcePlay(source.source);
  utils::check_al_error();

  source.buffer = buffer;
  buffer->play(source.source, source.loop);

  m_sound_stream_sources.push_back(std::move(source_ptr));

  return m_sound_stream_sources.back().get();
}

// void AudioManager::play(SoundSource& source)
// {
//   if (source.state == SoundState::Initial)
//   {
//     const auto& buffer = m_asset_manager.get<SoundBuffer>(source.resource_id);
//
//     alGenSources(1, &source.source);
//     utils::check_al_error();
//     alSourcef(source.source, AL_PITCH, 1.0f);
//     alSourcef(source.source, AL_GAIN, 1.0f);
//     alSource3f(source.source, AL_POSITION, 0.0f, 0.0f, 0.0f);
//     alSource3f(source.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
//     alSource3f(source.source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
//     alSourcei(source.source, AL_SOURCE_RELATIVE, AL_TRUE);
//     alSourcei(source.source, AL_BUFFER, buffer->id);
//   }
//
//   ALenum state;
//   alGetSourcei(source.source, AL_SOURCE_STATE, &state);
//
//   if (state == AL_PLAYING)
//   {
//     return;
//   }
//
//   source.state = SoundState::Playing;
//   alSourcei(source.source, AL_LOOPING, source.loop);
//   alSourcePlay(source.source);
// }

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

void AudioManager::pause(SoundStreamSource& source)
{
  assert(source.buffer != nullptr);
  source.buffer->pause(source.source);
}

void AudioManager::resume(SoundStreamSource& source)
{
  assert(source.buffer != nullptr);
  source.buffer->resume(source.source);
}

void AudioManager::stop(SoundStreamSource& source)
{
  source.state = SoundState::FadingOut;
  // assert(source.buffer != nullptr);
  // source.buffer->stop(source.source);
}

void AudioManager::update()
{
  for (auto& source : m_sound_sources)
  {
    ALenum al_state;
    alGetSourcei(source.source, AL_SOURCE_STATE, &al_state);
    source.state = utils::al_state_to_sound_state(al_state);

    if (source.state == SoundState::Stopped)
    {
      alDeleteSources(1, &source.source);
    }
  }

  for (auto& source_ptr : m_sound_stream_sources)
  {
    auto& source = *source_ptr;
    source.buffer->update(source.source);

    if (source.state == SoundState::FadingIn)
    {
      source.gain += 0.01f;

      if (source.gain >= 1.0f)
      {
        source.gain = 1.0f;
        source.state = SoundState::Playing;
      }

      alSourcef(source.source, AL_GAIN, source.gain);
    }
    else if (source.state == SoundState::FadingOut)
    {
      source.gain -= 0.01f;

      if (source.gain <= 0.0f)
      {
        source.gain = 0.0f;
        source.buffer->stop(source.source);
        source.state = SoundState::Stopped;
      }

      alSourcef(source.source, AL_GAIN, source.gain);
    }
    else
    {
      ALenum al_state;
      alGetSourcei(source.source, AL_SOURCE_STATE, &al_state);
      source.state = utils::al_state_to_sound_state(al_state);
    }

    if (source.state == SoundState::Stopped)
    {
      alDeleteSources(1, &source.source);
    }
  }

  std::erase_if(m_sound_sources, [](const auto& source) { return source.state == SoundState::Stopped; });
  std::erase_if(m_sound_stream_sources, [](const auto& source) { return source->state == SoundState::Stopped; });
}
}  // namespace dl::audio
