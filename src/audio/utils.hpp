#pragma once

#include <spdlog/spdlog.h>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

bool check_al_errors(const std::string& filename, const std::uint_fast32_t line)
{
  ALCenum error = alGetError();
  if (error != AL_NO_ERROR)
  {
    spdlog::critical("***ERROR*** ({}: {})", filename, line);
    switch (error)
    {
    case AL_INVALID_NAME:
      spdlog::critical("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
      return false;
    case AL_INVALID_ENUM:
      spdlog::critical("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
      return false;
    case AL_INVALID_VALUE:
      spdlog::critical("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
      return false;
    case AL_INVALID_OPERATION:
      spdlog::critical("AL_INVALID_OPERATION: the requested operation is not valid");
      return false;
    case AL_OUT_OF_MEMORY:
      spdlog::critical("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
      return false;
    default:
      spdlog::critical("UNKNOWN AL ERROR: {}", error);
      return false;
    }
  }
  return true;
}

bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
{
  ALCenum error = alcGetError(device);
  if (error != ALC_NO_ERROR)
  {
    spdlog::critical("***ERROR*** ({}: {})", filename, line);
    switch (error)
    {
    case ALC_INVALID_VALUE:
      spdlog::critical("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
      return false;
    case ALC_INVALID_DEVICE:
      spdlog::critical("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
      return false;
    case ALC_INVALID_CONTEXT:
      spdlog::critical("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
      return false;
    case ALC_INVALID_ENUM:
      spdlog::critical("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
      return false;
    case ALC_OUT_OF_MEMORY:
      spdlog::critical("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
      return false;
    default:
      spdlog::critical("UNKNOWN ALC ERROR: {}", error);
      return false;
    }
  }

  return true;
}

template <typename alFunction, typename... Params>
auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params) ->
    typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
{
  auto ret = function(std::forward<Params>(params)...);
  check_al_errors(filename, line);
  return ret;
}

template <typename alFunction, typename... Params>
auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params) ->
    typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
  function(std::forward<Params>(params)...);
  return check_al_errors(filename, line);
}

template <typename alcFunction, typename... Params>
auto alcCallImpl(
    const char* filename, const std::uint_fast32_t line, alcFunction function, ALCdevice* device, Params... params) ->
    typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
  function(std::forward<Params>(params)...);
  return check_alc_errors(filename, line, device);
}

template <typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char* filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ReturnType& returnValue,
                 ALCdevice* device,
                 Params... params) ->
    typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
  returnValue = function(std::forward<Params>(params)...);
  return check_alc_errors(filename, line, device);
}
