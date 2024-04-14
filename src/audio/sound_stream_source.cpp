// #include "./sound_stream_source.hpp"
//
// #include <spdlog/spdlog.h>
//
// #include "audio/utils.hpp"
//
// namespace dl::audio
// {
// SoundStreamSource::SoundStreamSource(const std::string& filepath) : m_ogg(OggData{filepath})
// {
//   m_reseted = true;
//
//   alSourcef(m_source, AL_PITCH, 1.0f);
//   alSourcef(m_source, AL_GAIN, 1.0f);
//   alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
//   alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
//   alSource3f(m_source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
//   alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
// }
//
// void SoundStreamSource::play(const bool loop)
// {
//   if (is_playing())
//   {
//     return;
//   }
//
//   m_loop = loop;
//
//   for (int i = 0; i < STREAM_BUFFERS; ++i)
//   {
//     if (!m_stream_buffer(m_buffers[i]))
//     {
//       return;
//     }
//   }
//
//   alSourceQueueBuffers(m_source, STREAM_BUFFERS, m_buffers);
//   alSourcePlay(m_source);
//   m_reseted = false;
// }
//
// void SoundStreamSource::pause()
// {
//   if (is_playing())
//   {
//     alSourcePause(m_source);
//   }
// }
//
// void SoundStreamSource::resume()
// {
//   if (is_paused())
//   {
//     alSourcePlay(m_source);
//   }
// }
//
// void SoundStreamSource::stop()
// {
//   if (is_playing() || is_paused())
//   {
//     alSourceStop(m_source);
//     ov_raw_seek(&m_ogg.file_data, 0);
//     m_reseted = true;
//     m_empty_queue();
//   }
// }
//
// void SoundStreamSource::update()
// {
//   alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
//
//   if (m_state == AL_PAUSED)
//   {
//     return;
//   }
//
//   int processed;
//   alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
//
//   while (processed > 0)
//   {
//     ALuint buffer;
//     alSourceUnqueueBuffers(m_source, 1, &buffer);
//     check_al_error();
//
//     bool active = m_stream_buffer(buffer);
//
//     if (active)
//     {
//       alSourceQueueBuffers(m_source, 1, &buffer);
//     }
//
//     --processed;
//   }
//
//   if (m_state == AL_STOPPED)
//   {
//     if (m_loop)
//     {
//       ov_raw_seek(&m_ogg.file_data, 0);
//       play(m_loop);
//     }
//     else if (!m_reseted)
//     {
//       m_reseted = true;
//       ov_raw_seek(&m_ogg.file_data, 0);
//     }
//   }
// }
//
// bool SoundStreamSource::is_paused()
// {
//   return (m_state == AL_PAUSED);
// }
//
// bool SoundStreamSource::is_stopped()
// {
//   return (m_state == AL_STOPPED || m_state == AL_INITIAL);
// }
//
// bool SoundStreamSource::is_playing()
// {
//   return (m_state == AL_PLAYING);
// }
//
// void SoundStreamSource::destroy()
// {
//   alSourceStop(m_source);
//   m_empty_queue();
//   alDeleteSources(1, &m_source);
//   alDeleteBuffers(STREAM_BUFFERS, m_buffers);
// }
//
// bool SoundStreamSource::m_stream_buffer(ALuint buffer)
// {
//   char data[AUDIO_BUFFER_SIZE];
//   int size = 0;
//   int section;
//
//   while (size < AUDIO_BUFFER_SIZE)
//   {
//     const int result = ov_read(&m_ogg.file_data, data + size, AUDIO_BUFFER_SIZE - size, 0, 2, 1, &section);
//
//     if (result > 0)
//     {
//       size += result;
//     }
//     else if (result < 0)
//     {
//       check_ogg_error(result);
//       return false;
//     }
//     else
//     {
//       break;
//     }
//   }
//   if (size == 0)
//   {
//     return false;
//   }
//
//   alBufferData(buffer, m_ogg.format, data, size, m_ogg.metadata->rate);
//   check_al_error();
//   return true;
// }
//
// void SoundStreamSource::m_empty_queue()
// {
//   int queued;
//   alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
//
//   while (queued--)
//   {
//     ALuint buffer;
//     alSourceUnqueueBuffers(m_source, 1, &buffer);
//     check_al_error();
//   }
// }
// }  // namespace dl::audio
//
