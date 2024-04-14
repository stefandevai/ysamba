#include "./ogg_data.hpp"

#include <spdlog/spdlog.h>

// #include <iostream>

namespace
{
std::string error_to_string(int code)
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
OggData::OggData() {}

OggData::~OggData()
{
  if (has_loaded)
  {
    fclose(file);
    ov_clear(&file_data);
  }
}

void OggData::load(const std::string& filepath)
{
  if (!(file = fopen(filepath.c_str(), "rb")))
  {
    spdlog::critical("Failed to open ogg file");
    return;
  }

  int result = ov_open(file, &file_data, NULL, 0);

  if (result < 0)
  {
    spdlog::critical("Failed to open ogg stream: {}", error_to_string(result));
    fclose(file);
    return;
  }

  metadata = ov_info(&file_data, -1);
  size = ov_pcm_total(&file_data, -1) * metadata->channels * 2;

  if (metadata->channels == 1)
  {
    format = AL_FORMAT_MONO16;
  }
  else
  {
    format = AL_FORMAT_STEREO16;
  }

  has_loaded = true;
}
}  // namespace dl::audio
