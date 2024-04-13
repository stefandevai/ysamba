#pragma once

#include <AL/al.h>

#define AUDIO_BUFFER_SIZE (4096 * 8)

namespace dl
{
namespace audio
{
  class Playable
  {
  public:
    inline virtual ~Playable() {}
    virtual void Play (const bool& loop = false)      = 0;
    virtual void Pause (const bool& fadeOut = false)  = 0;
    virtual void Resume (const bool& fadeOut = false) = 0;
    virtual void Stop (const bool& fadeOut = false)   = 0;
    virtual void Update()                             = 0;
    virtual bool IsInitialized()                      = 0;

  protected:
    ALuint Source;
    ALenum Format;
    ALint State;
    bool Loop = false;

    inline Playable() {}
  };
} // namespace audio
} // namespace stella
