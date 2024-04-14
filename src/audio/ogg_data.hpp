#pragma once

#include <AL/al.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <string>

namespace dl::audio
{
class OggData
{
 public:
  FILE* file = nullptr;
  OggVorbis_File file_data;
  vorbis_info* metadata = nullptr;
  ALenum format;
  size_t size = -1;
  bool has_loaded = false;

  OggData();
  ~OggData();

  void load(const std::string& filepath);
};
}  // namespace dl::audio
