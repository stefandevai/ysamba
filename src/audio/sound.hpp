#include <AL/al.h>

#include <string>

#include "audio/ogg_data.hpp"

namespace dl::audio
{
class Sound
{
 public:
  explicit Sound(const std::string& filepath);

  void play(bool loop);
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
  ALuint m_buffer;
  ALuint m_source;
};
}  // namespace dl::audio
