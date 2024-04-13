#pragma once

#include <AL/al.h>

#include <string>

// #include <ogg/ogg.h>
// #include <vorbis/codec.h>
// #include <vorbis/vorbisfile.h>
#include "audio/ogg_data.hpp"

namespace dl::audio
{
constexpr uint32_t STREAM_BUFFERS = 4;
constexpr size_t BUFFER_SIZE = 65536;  // 32kb

class Stream
{
 public:
  std::unique_ptr<OggData> ogg = nullptr;
  // FILE* file = nullptr;
  // OggVorbis_File file_data;
  // vorbis_info* metadata = nullptr;
  ALuint buffers[STREAM_BUFFERS];
  // size_t size = -1;

  Stream(const std::string& filepath);
  ~Stream();

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

  // void openFile (const char* filepath);
  // const void displayInfo() const;
  // bool streamBuffer (ALuint buffer);
  // void emptyQueue();
  // void clean();
};
}  // namespace dl::audio
