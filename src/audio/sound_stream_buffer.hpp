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
  explicit SoundStreamBuffer(const std::string& filepath);

  void play(bool loop = false);
  void pause();
  void resume();
  void stop();
  void update();
  bool is_playing();
  bool is_stopped();
  bool is_paused();
  void destroy();

 private:
  ALenum m_state = AL_STOPPED;
  bool m_loop = false;
  bool m_reseted;
  OggData m_ogg;
  ALuint m_buffers[STREAM_BUFFERS];
  ALuint m_source;

  bool m_stream_buffer(ALuint buffer);
  void m_empty_queue();
};
}  // namespace dl::audio
