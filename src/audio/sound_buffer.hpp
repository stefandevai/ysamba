#include <AL/al.h>

#include <string>

namespace dl::audio
{
class SoundBuffer
{
 public:
  explicit SoundBuffer(const std::string& filepath);

  void play(bool loop = false);
  void pause();
  void resume();
  void stop();
  bool is_playing();
  bool is_stopped();
  bool is_paused();
  void destroy();

 private:
  ALenum m_state = AL_STOPPED;
  bool m_loop = false;
  ALuint m_buffer;
  ALuint m_source;
};
}  // namespace dl::audio
