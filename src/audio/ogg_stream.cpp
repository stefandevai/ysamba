#include "./ogg_stream.hpp"

#include <spdlog/spdlog.h>

namespace
{
void check_al_error()
{
  const int error = alGetError();

  if (error != AL_NO_ERROR)
  {
    spdlog::critical("OpenAL error:");
    switch (error)
    {
    case AL_INVALID_NAME:
      spdlog::critical("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
      break;
    case AL_INVALID_ENUM:
      spdlog::critical("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
      break;
    case AL_INVALID_VALUE:
      spdlog::critical("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
      break;
    case AL_INVALID_OPERATION:
      spdlog::critical("AL_INVALID_OPERATION: the requested operation is not valid");
      break;
    case AL_OUT_OF_MEMORY:
      spdlog::critical("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
      break;
    default:
      spdlog::critical("UNKNOWN AL ERROR: {}", error);
      break;
    }
  }
}

std::string ogg_error_to_string(const int code)
{
  switch (code)
  {
  case OV_EREAD:
    return "Read from media.";
  case OV_ENOTVORBIS:
    return "Not vorbis data.";
  case OV_EVERSION:
    return "Vorbis version mismatch";
  case OV_EBADHEADER:
    return "Invalid vorbis header.";
  case OV_EFAULT:
    return "Internal logic fault";
  default:
    return "Unknown Ogg error.";
  }
}
}  // namespace

namespace dl::audio
{
OggStream::OggStream(const std::string& filepath) : m_ogg(OggData{filepath})
{
  m_reseted = true;

  alGenBuffers(STREAM_BUFFERSS, m_buffers);
  check_al_error();
  alGenSources(1, &m_source);
  check_al_error();

  alSource3f(m_source, AL_POSITION, 0.0, 0.0, 0.0);
  alSource3f(m_source, AL_VELOCITY, 0.0, 0.0, 0.0);
  alSource3f(m_source, AL_DIRECTION, 0.0, 0.0, 0.0);
  alSourcef(m_source, AL_ROLLOFF_FACTOR, 0.0);
  alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
}

void OggStream::play(const bool loop)
{
  if (is_playing())
    return;
  m_loop = loop;
  for (int i = 0; i < STREAM_BUFFERSS; ++i)
    if (!m_stream_buffer(m_buffers[i]))
      return;

  alSourceQueueBuffers(m_source, STREAM_BUFFERSS, m_buffers);
  alSourcePlay(m_source);
  m_reseted = false;
}

void OggStream::pause()
{
  if (is_playing())
  {
    alSourcePause(m_source);
  }
}

void OggStream::resume()
{
  if (is_paused())
  {
    alSourcePlay(m_source);
  }
}

void OggStream::stop()
{
  if (is_playing() || is_paused())
  {
    alSourceStop(m_source);
    ov_raw_seek(&m_ogg.file_data, 0);
    m_reseted = true;
    m_empty_queue();
  }
}

void OggStream::update()
{
  alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);

  if (m_state == AL_PAUSED)
  {
    return;
  }

  int processed;
  // bool active = true;
  alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
  while (processed--)
  {
    ALuint buffer;
    alSourceUnqueueBuffers(m_source, 1, &buffer);
    check_al_error();

    bool active = m_stream_buffer(buffer);
    if (active)
    {
      alSourceQueueBuffers(m_source, 1, &buffer);
      check_al_error();
    }
  }

  if (m_state == AL_STOPPED)
  {
    if (m_loop)
    {
      ov_raw_seek(&m_ogg.file_data, 0);
      play(m_loop);
    }
    else if (!m_reseted)
    {
      m_reseted = true;
      ov_raw_seek(&m_ogg.file_data, 0);
    }
  }
}

bool OggStream::is_paused()
{
  return (m_state == AL_PAUSED);
}

bool OggStream::is_stopped()
{
  return (m_state == AL_STOPPED || m_state == AL_INITIAL);
}

bool OggStream::is_playing()
{
  return (m_state == AL_PLAYING);
}

void OggStream::destroy()
{
  alSourceStop(m_source);
  m_empty_queue();
  alDeleteSources(1, &m_source);
  alDeleteBuffers(STREAM_BUFFERSS, m_buffers);
}

bool OggStream::m_stream_buffer(ALuint buffer)
{
  char data[AUDIO_BUFFER_SIZE];
  int size = 0;
  int section;

  while (size < AUDIO_BUFFER_SIZE)
  {
    const int result = ov_read(&m_ogg.file_data, data + size, AUDIO_BUFFER_SIZE - size, 0, 2, 1, &section);

    if (result > 0)
    {
      size += result;
    }
    else if (result < 0)
    {
      spdlog::critical(ogg_error_to_string(result));
      return false;
    }
    else
    {
      break;
    }
  }
  if (size == 0)
  {
    return false;
  }

  alBufferData(buffer, m_ogg.format, data, size, m_ogg.metadata->rate);
  check_al_error();
  return true;
}

void OggStream::m_empty_queue()
{
  int queued;
  alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);

  while (queued--)
  {
    ALuint buffer;
    alSourceUnqueueBuffers(m_source, 1, &buffer);
    check_al_error();
  }
}
}  // namespace dl::audio
