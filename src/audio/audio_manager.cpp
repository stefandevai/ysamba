#include "./audio_manager.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <spdlog/spdlog.h>

#include "audio/utils.hpp"

namespace dl::audio
{
AudioManager::AudioManager()
{
  int error;
  ALCdevice* m_device = alcOpenDevice(nullptr);

  if ((error = alcGetError(m_device)) != AL_NO_ERROR)
  {
    spdlog::critical("Error loading ALC device: {}", error);
  }

  if (m_device == nullptr)
  {
    spdlog::critical("Failed to open OpenAL device");
    return;
  }

  m_context = alcCreateContext(m_device, nullptr);
  if ((error = alcGetError(m_device)) != AL_NO_ERROR)
  {
    spdlog::critical("Error loading ALC context: {}", error);
  }
  // if(!alcCall(alcCreateContext, m_context, m_device, m_device, nullptr) || !m_context)
  // {
  //   spdlog::critical("Failed to create OpenAL context");
  //   alcCloseDevice(m_device);
  //   return;
  // }

  alcMakeContextCurrent(m_context);
  if ((error = alcGetError(m_device)) != AL_NO_ERROR)
  {
    spdlog::critical("Error loading ALC context curr: {}", error);
  }
  // ALCboolean contextMadeCurrent = false;
  // if(!alcCall(alcMakeContextCurrent, contextMadeCurrent, m_device, m_context)
  //    || contextMadeCurrent != ALC_TRUE)
  // {
  //   spdlog::critical("Failed to make OpenAL context current");
  //   alcCloseDevice(m_device);
  //   return;
  // }

  m_stream = std::make_unique<OggStream>("data/audio/theme.ogg");
  m_stream->play(false);
}

AudioManager::~AudioManager()
{
  m_stream->destroy();
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

void AudioManager::test()
{
  // int error = 0;
  //
  // ALuint buffer;
  // {
  //   auto stream = std::make_unique<OggData>("data/audio/theme.ogg");
  //
  //   ALenum audio_format;
  //
  //   if (stream->metadata->channels == 1)
  //   {
  //     audio_format = AL_FORMAT_MONO16;
  //   }
  //   else
  //   {
  //     audio_format = AL_FORMAT_STEREO16;
  //   }
  //
  //   alGenBuffers(1, &buffer);
  //   alBufferData(buffer, audio_format, stream->buffer, stream->size, stream->metadata->rate);
  //
  //   if ((error = alGetError()) != AL_NO_ERROR)
  //   {
  //     spdlog::critical("Error loading AL buffer: {}", error);
  //   }
  // }
  //
  // ALuint source;
  // alGenSources(1, &source);
  // alSourcef(source, AL_PITCH, 1);
  // alSourcef(source, AL_GAIN, 1.0f);
  // alSource3f(source, AL_POSITION, 0, 0, 0);
  // alSource3f(source, AL_VELOCITY, 0, 0, 0);
  // alSourcei(source, AL_LOOPING, AL_FALSE);
  // alSourcei(source, AL_BUFFER, buffer);
  //
  // alSourcePlay(source);
  //
  // ALint state = AL_PLAYING;
  // while(state == AL_PLAYING)
  // {
  //   alGetSourcei(source, AL_SOURCE_STATE, &state);
  // }
  //
  // alDeleteSources(1, &source);
  // alDeleteBuffers(1, &buffer);
}

void AudioManager::update()
{
  m_stream->update();
}
}  // namespace dl::audio
