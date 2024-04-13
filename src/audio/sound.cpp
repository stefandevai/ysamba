#include "./sound.hpp"

#include <spdlog/spdlog.h>

#include "audio/utils.hpp"

namespace dl::audio
{
Sound::Sound(const std::string& filepath)
{
  m_reseted = true;

  {
    OggData ogg{filepath};

    char ogg_buffer[ogg.size];

    size_t bytes_read = -1, offset = 0;
    int sel = 0;

    while (bytes_read != 0)
    {
      bytes_read = ov_read(&ogg.file_data, ogg_buffer + offset, 4096, 0, 2, 1, &sel);

      if (bytes_read < 0)
      {
        spdlog::critical("Error reading OGG file");
        return;
      }

      offset += bytes_read;
    }

    alGenBuffers(1, &m_buffer);
    alBufferData(m_buffer, ogg.format, ogg_buffer, ogg.size, ogg.metadata->rate);
    check_al_error();
  }

  alGenSources(1, &m_source);
  alSourcef(m_source, AL_PITCH, 1);
  alSourcef(m_source, AL_GAIN, 1.0f);
  alSource3f(m_source, AL_POSITION, 0, 0, 0);
  alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
  alSourcei(m_source, AL_BUFFER, m_buffer);
}

void Sound::play(const bool loop)
{
  if (is_playing())
  {
    return;
  }

  m_loop = loop;
  alSourcei(m_source, AL_LOOPING, loop);
  alSourcePlay(m_source);
  m_reseted = false;
}

void Sound::pause()
{
  if (is_playing())
  {
    alSourcePause(m_source);
  }
}

void Sound::resume()
{
  if (is_paused())
  {
    alSourcePlay(m_source);
  }
}

void Sound::stop()
{
  if (is_playing() || is_paused())
  {
    alSourceStop(m_source);
    m_reseted = true;
  }
}

void Sound::update()
{
  alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);

  if (m_state == AL_PAUSED)
  {
    return;
  }

  if (m_state == AL_STOPPED)
  {
    if (m_loop)
    {
      play(m_loop);
    }
    else if (!m_reseted)
    {
      m_reseted = true;
    }
  }
}

bool Sound::is_paused()
{
  return (m_state == AL_PAUSED);
}

bool Sound::is_stopped()
{
  return (m_state == AL_STOPPED || m_state == AL_INITIAL);
}

bool Sound::is_playing()
{
  return (m_state == AL_PLAYING);
}

void Sound::destroy()
{
  alSourceStop(m_source);
  alDeleteSources(1, &m_source);
  alDeleteBuffers(1, &m_buffer);
}
}  // namespace dl::audio
