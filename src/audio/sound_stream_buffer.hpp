#pragma once

#include <AL/al.h>

#include <string>

#include "audio/ogg_data.hpp"

namespace dl::audio
{
constexpr int64_t AUDIO_BUFFER_SIZE = 65536;  // 32kb
constexpr int32_t STREAM_BUFFERS = 4;

class SoundStreamBuffer
{
 public:
  bool has_loaded = false;

  explicit SoundStreamBuffer(const std::string filepath);

  void load();
  void play(ALuint source, bool loop = false);
  void pause(ALuint source);
  void resume(ALuint source);
  void stop(ALuint source);
  void update(ALuint source);
  bool is_playing();
  bool is_stopped();
  bool is_paused();
  void destroy();
  bool fill_buffer(ALuint buffer);

 private:
  std::string m_filepath;
  ALenum m_state = AL_STOPPED;
  bool m_loop = false;
  bool m_reseted = true;
  OggData m_ogg{};
  ALuint m_buffers[STREAM_BUFFERS];
  // ALuint source;

  void m_empty_queue(ALuint source);
};
}  // namespace dl::audio
