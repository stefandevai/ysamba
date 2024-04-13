#pragma once

#include <AL/al.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <string>

#include "audio/ogg_data.hpp"

#define AUDIO_BUFFER_SIZE (4096 * 8)
#define STREAM_BUFFERSS 4

namespace dl::audio
{
class OggStream
{
 public:
  explicit OggStream(const std::string& filepath);

  void play(const bool loop);
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
  ALuint m_buffers[STREAM_BUFFERSS];
  ALuint m_source;

  bool m_stream_buffer(ALuint buffer);
  void m_empty_queue();
};
}  // namespace dl::audio
