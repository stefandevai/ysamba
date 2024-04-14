#pragma once

#include <AL/al.h>

#include <string>

namespace dl::audio
{
class SoundBuffer
{
 public:
  ALuint id;
  bool has_loaded = false;

  explicit SoundBuffer(const std::string& filepath);

  void load();
  void play(bool loop = false);
  void pause();
  void resume();
  void stop();
  bool is_playing();
  bool is_stopped();
  bool is_paused();
  void destroy();

 private:
  std::string m_filepath;
  ALenum m_state = AL_STOPPED;
  ALuint m_source;
};
}  // namespace dl::audio
