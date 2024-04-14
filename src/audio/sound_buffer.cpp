#include "./sound_buffer.hpp"

#include <spdlog/spdlog.h>

#include "audio/ogg_data.hpp"
#include "audio/utils.hpp"

namespace dl::audio
{
SoundBuffer::SoundBuffer(const std::string& filepath) : m_filepath(filepath) {}

void SoundBuffer::load()
{
  {
    OggData ogg{};
    ogg.load(m_filepath);

    char ogg_buffer[ogg.size];

    size_t bytes_decoded = -1, offset = 0;
    constexpr int length = 4096;
    constexpr int is_bigendian = 0;
    constexpr int word_size = 2;
    constexpr int is_signed_data = 1;
    int bitstream = 0;

    while (bytes_decoded != 0)
    {
      bytes_decoded
          = ov_read(&ogg.file_data, &ogg_buffer[offset], length, is_bigendian, word_size, is_signed_data, &bitstream);

      if (bytes_decoded < 0)
      {
        spdlog::critical("Error reading OGG file");
        return;
      }

      offset += bytes_decoded;
    }

    alGenBuffers(1, &id);
    alBufferData(id, ogg.format, ogg_buffer, ogg.size, ogg.metadata->rate);
    check_al_error();
  }

  alGenSources(1, &m_source);
  alSourcef(m_source, AL_PITCH, 1.0f);
  alSourcef(m_source, AL_GAIN, 1.0f);
  alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
  alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcei(m_source, AL_BUFFER, id);

  has_loaded = true;
}

void SoundBuffer::play(const bool loop)
{
  if (is_playing())
  {
    return;
  }

  alSourcei(m_source, AL_LOOPING, loop);
  alSourcePlay(m_source);
}

void SoundBuffer::pause()
{
  if (is_playing())
  {
    alSourcePause(m_source);
  }
}

void SoundBuffer::resume()
{
  if (is_paused())
  {
    alSourcePlay(m_source);
  }
}

void SoundBuffer::stop()
{
  if (is_playing() || is_paused())
  {
    alSourceStop(m_source);
  }
}

bool SoundBuffer::is_paused()
{
  alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PAUSED);
}

bool SoundBuffer::is_stopped()
{
  alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_STOPPED || m_state == AL_INITIAL);
}

bool SoundBuffer::is_playing()
{
  alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PLAYING);
}

void SoundBuffer::destroy()
{
  alSourceStop(m_source);
  alDeleteSources(1, &m_source);
  alDeleteBuffers(1, &id);
}
}  // namespace dl::audio
