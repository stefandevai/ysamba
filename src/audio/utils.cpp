#include "./utils.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>
#include <spdlog/spdlog.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace dl::audio
{
void check_alc_error(ALCdevice* device)
{
  const int error = alcGetError(device);

  if (error != ALC_NO_ERROR)
  {
    spdlog::critical("OpenAL Context error:");
    switch (error)
    {
    case ALC_INVALID_VALUE:
      spdlog::critical("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
      break;
    case ALC_INVALID_DEVICE:
      spdlog::critical("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
      break;
    case ALC_INVALID_CONTEXT:
      spdlog::critical("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
      break;
    case ALC_INVALID_ENUM:
      spdlog::critical("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
      break;
    case ALC_OUT_OF_MEMORY:
      spdlog::critical("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
      break;
    default:
      spdlog::critical("UNKNOWN ALC ERROR: {}", error);
      break;
    }
  }
}

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

void check_ogg_error(const int code)
{
  switch (code)
  {
    spdlog::critical("OGG error:");
  case OV_EREAD:
    spdlog::critical("OV_EREAD: Read from media.");
    break;
  case OV_ENOTVORBIS:
    spdlog::critical("OV_ENOTVORBIS: No vorbis data.");
    break;
  case OV_EVERSION:
    spdlog::critical("OV_EVERSION: Vorbis version mismatch");
    break;
  case OV_EBADHEADER:
    spdlog::critical("OV_EBADHEADER: Invalid vorbis header.");
    break;
  case OV_EFAULT:
    spdlog::critical("OV_EFAULT: Internal logic fault");
    break;
  default:
    spdlog::critical("UNKNOWN OGG ERROR: {}", code);
    break;
  }
}
}  // namespace dl::audio
