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
  void play(ALuint source, bool loop = false);
  void pause(ALuint source);
  void resume(ALuint source);
  void stop(ALuint source);
  bool is_playing(ALuint source);
  bool is_stopped(ALuint source);
  bool is_paused(ALuint source);
  void destroy();

 private:
  std::string m_filepath;
  ALenum m_state = AL_STOPPED;
};
}  // namespace dl::audio
