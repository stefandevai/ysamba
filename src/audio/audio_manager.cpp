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

  if ((error = alcGetError(m_device)) != AL_NO_ERROR) {
		spdlog::critical("Error loading ALC device: {}", error);
	}

  if(m_device == nullptr)
  {
    spdlog::critical("Failed to open OpenAL device");
    return;
  }

  m_context = alcCreateContext(m_device, nullptr);
  if ((error = alcGetError(m_device)) != AL_NO_ERROR) {
		spdlog::critical("Error loading ALC context: {}", error);
	}
  // if(!alcCall(alcCreateContext, m_context, m_device, m_device, nullptr) || !m_context)
  // {
  //   spdlog::critical("Failed to create OpenAL context");
  //   alcCloseDevice(m_device);
  //   return;
  // }

  alcMakeContextCurrent(m_context);
  if ((error = alcGetError(m_device)) != AL_NO_ERROR) {
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
}

AudioManager::~AudioManager()
{
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

void AudioManager::test()
{
  int error = 0;
  // Load ogg file
  ALenum audio_format;
  m_stream = std::make_unique<OggData>("data/audio/theme.ogg");

  auto channels = m_stream->metadata->channels;
  if (channels == 1)
  {
    audio_format = AL_FORMAT_MONO16;
  }
  else
  {
    audio_format = AL_FORMAT_STEREO16;
  }

  size_t data_len = ov_pcm_total(&m_stream->data, -1) * m_stream->metadata->channels * 2;

  char* pcmout;
  pcmout = (char*)malloc(data_len);

  size_t size = -1, offset = 0;
  int sel = 0;
  while (size != 0)
  {
    size = ov_read(&m_stream->data, pcmout + offset, 4096, 0, 2, 1, &sel);
		if(size < 0)
			puts("Faulty ogg file :o"); // use https://xiph.org/vorbis/doc/vorbisfile/ov_read.html for handling enums
    offset += size;
  }

  // Create buffer
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, audio_format, pcmout, data_len, m_stream->metadata->rate);

  if ((error = alGetError()) != AL_NO_ERROR) {
		spdlog::critical("Error loading AL buffer: {}", error);
	}

  ALuint source;
  alGenSources(1, &source);
  alSourcef(source, AL_PITCH, 1);
  alSourcef(source, AL_GAIN, 1.0f);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_BUFFER, buffer);

  alSourcePlay(source);

  ALint state = AL_PLAYING;

  while(state == AL_PLAYING)
  {
    alGetSourcei(source, AL_SOURCE_STATE, &state);
  }

  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  free(pcmout);
}

void AudioManager::update()
{

}
}
