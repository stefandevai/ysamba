#pragma once

#include <AL/al.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <string>

// #define STREAM_BUFFERS 4

namespace dl::audio
{
class OggData
{
 public:
  FILE* file = nullptr;
  OggVorbis_File file_data;
  vorbis_info* metadata = nullptr;
  ALenum format;
  // char* buffer = nullptr;
  size_t size = -1;

  OggData(const std::string& filepath);
  ~OggData();

  // void Play (const bool& loop) override;
  // void Pause (const bool& fadeOut = false) override;
  // void Resume (const bool& fadeOut = false) override;
  // void Stop (const bool& fadeOut = false) override;
  // void Update() override;
  // bool IsPlaying();
  // bool IsStoped();
  // bool IsPaused();
  // virtual bool IsInitialized() const override;
  // inline bool IsInitialized() override { return this->StreamOpened; }

 private:
  // const char* FilePath;
  // bool StreamOpened, Reseted;
  // FILE *OggFile;
  // vorbis_comment* VorbisComment = nullptr;
  // ALuint Buffers[STREAM_BUFFERS];

  // void openFile (const char* filepath);
  // const void displayInfo() const;
  // bool streamBuffer (ALuint buffer);
  // void emptyQueue();
  // void clean();
};
}  // namespace dl::audio
