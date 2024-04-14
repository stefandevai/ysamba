#include "./sound_stream_buffer.hpp"

#include <spdlog/spdlog.h>

#include "audio/utils.hpp"

namespace dl::audio
{
SoundStreamBuffer::SoundStreamBuffer(const std::string filepath) : m_filepath(std::move(filepath)) {}

void SoundStreamBuffer::load()
{
  m_ogg.load(m_filepath);

  alGenBuffers(STREAM_BUFFERS, m_buffers);
  utils::check_al_error();

  has_loaded = true;
}

void SoundStreamBuffer::play(ALuint source, const bool loop)
{
  if (is_playing(source))
  {
    return;
  }

  m_loop = loop;

  for (int i = 0; i < STREAM_BUFFERS; ++i)
  {
    if (!fill_buffer(m_buffers[i]))
    {
      return;
    }
  }

  alSourceQueueBuffers(source, STREAM_BUFFERS, m_buffers);
  alSourcePlay(source);
  m_reseted = false;
}

void SoundStreamBuffer::pause(ALuint source)
{
  if (is_playing(source))
  {
    alSourcePause(source);
  }
}

void SoundStreamBuffer::resume(ALuint source)
{
  if (is_paused(source))
  {
    alSourcePlay(source);
  }
}

void SoundStreamBuffer::stop(ALuint source)
{
  alSourceStop(source);
  ov_raw_seek(&m_ogg.file_data, 0);
  m_empty_queue(source);
  m_reseted = true;
}

void SoundStreamBuffer::update(ALuint source)
{
  ALenum m_state;
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);

  if (m_state == AL_PAUSED || m_state == AL_INITIAL)
  {
    return;
  }

  int processed;
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

  while (processed > 0)
  {
    ALuint buffer;
    alSourceUnqueueBuffers(source, 1, &buffer);
    utils::check_al_error();

    bool active = fill_buffer(buffer);

    if (active)
    {
      alSourceQueueBuffers(source, 1, &buffer);
    }

    --processed;
  }

  if (m_state == AL_STOPPED)
  {
    if (m_loop)
    {
      ov_raw_seek(&m_ogg.file_data, 0);
      play(source, m_loop);
    }
    else if (!m_reseted)
    {
      m_reseted = true;
      ov_raw_seek(&m_ogg.file_data, 0);
    }
  }
}

bool SoundStreamBuffer::is_paused(ALuint source)
{
  ALenum m_state;
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PAUSED);
}

bool SoundStreamBuffer::is_stopped(ALuint source)
{
  ALenum m_state;
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_STOPPED || m_state == AL_INITIAL);
}

bool SoundStreamBuffer::is_playing(ALuint source)
{
  ALenum m_state;
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PLAYING);
}

void SoundStreamBuffer::destroy()
{
  // alSourceStop(source);
  // m_empty_queue();
  // alDeleteSources(1, &source);
  alDeleteBuffers(STREAM_BUFFERS, m_buffers);
}

bool SoundStreamBuffer::fill_buffer(ALuint buffer)
{
  char data[AUDIO_BUFFER_SIZE];
  int size = 0;
  int section;

  while (size < AUDIO_BUFFER_SIZE)
  {
    const int result = ov_read(&m_ogg.file_data, data + size, AUDIO_BUFFER_SIZE - size, 0, 2, 1, &section);

    if (result > 0)
    {
      size += result;
    }
    else if (result < 0)
    {
      utils::check_ogg_error(result);
      return false;
    }
    else
    {
      break;
    }
  }
  if (size == 0)
  {
    return false;
  }

  alBufferData(buffer, m_ogg.format, data, size, m_ogg.metadata->rate);
  utils::check_al_error();
  return true;
}

void SoundStreamBuffer::m_empty_queue(ALuint source)
{
  int queued;
  alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

  while (queued--)
  {
    ALuint buffer;
    alSourceUnqueueBuffers(source, 1, &buffer);
    utils::check_al_error();
  }
}
}  // namespace dl::audio
