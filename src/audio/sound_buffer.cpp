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
    utils::check_al_error();
  }

  has_loaded = true;
}

void SoundBuffer::play(ALuint source, const bool loop)
{
  if (is_playing(source))
  {
    return;
  }

  alSourcei(source, AL_LOOPING, loop);
  alSourcePlay(source);
}

void SoundBuffer::pause(ALuint source)
{
  if (is_playing(source))
  {
    alSourcePause(source);
  }
}

void SoundBuffer::resume(ALuint source)
{
  if (is_paused(source))
  {
    alSourcePlay(source);
  }
}

void SoundBuffer::stop(ALuint source)
{
  if (is_playing(source) || is_paused(source))
  {
    alSourceStop(source);
  }
}

bool SoundBuffer::is_paused(ALuint source)
{
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PAUSED);
}

bool SoundBuffer::is_stopped(ALuint source)
{
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_STOPPED || m_state == AL_INITIAL);
}

bool SoundBuffer::is_playing(ALuint source)
{
  alGetSourcei(source, AL_SOURCE_STATE, &m_state);
  return (m_state == AL_PLAYING);
}

void SoundBuffer::destroy()
{
  if (has_loaded)
  {
    alDeleteBuffers(1, &id);
    has_loaded = false;
  }
}
}  // namespace dl::audio
