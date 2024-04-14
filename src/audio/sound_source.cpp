// #include "./sound_source.hpp"
//
// #include <spdlog/spdlog.h>
//
// namespace dl::audio
// {
// SoundSource::SoundSource(const std::string& filepath)
// {
//   alGenSources(1, &m_source);
//   alSourcef(m_source, AL_PITCH, 1.0f);
//   alSourcef(m_source, AL_GAIN, 1.0f);
//   alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
//   alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
//   alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
//   alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
//   alSourcei(m_source, AL_BUFFER, m_buffer);
// }
//
// void SoundSource::play(const bool loop)
// {
//   if (is_playing())
//   {
//     return;
//   }
//
//   alSourcei(m_source, AL_LOOPING, loop);
//   alSourcePlay(m_source);
// }
//
// void SoundSource::pause()
// {
//   if (is_playing())
//   {
//     alSourcePause(m_source);
//   }
// }
//
// void SoundSource::resume()
// {
//   if (is_paused())
//   {
//     alSourcePlay(m_source);
//   }
// }
//
// void SoundSource::stop()
// {
//   if (is_playing() || is_paused())
//   {
//     alSourceStop(m_source);
//   }
// }
//
// bool SoundSource::is_paused()
// {
//   alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
//   return (m_state == AL_PAUSED);
// }
//
// bool SoundSource::is_stopped()
// {
//   alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
//   return (m_state == AL_STOPPED || m_state == AL_INITIAL);
// }
//
// bool SoundSource::is_playing()
// {
//   alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
//   return (m_state == AL_PLAYING);
// }
//
// void SoundSource::destroy()
// {
//   alSourceStop(m_source);
//   alDeleteSources(1, &m_source);
// }
// }  // namespace dl::audio
//
