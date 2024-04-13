#include "./ogg_data.hpp"

#include <spdlog/spdlog.h>

// #include <iostream>

namespace
{
std::string error_to_string (int code)
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
} // namespace

namespace dl::audio
{
    FILE* file = nullptr;
  OggData::OggData(std::string filepath)
  {
    if (!(file = fopen(filepath.c_str(), "rb")))
    {
      spdlog::critical("Failed to open ogg file");
      return;
    }

    int result = ov_open(file, &data, NULL, 0);
    if (result < 0)
    {
      spdlog::critical("Failed to open ogg stream: {}", error_to_string(result));
      fclose (file);
      return;
    }


    metadata = ov_info(&data, -1);
    // if (this->VorbisInfo->channels == 1)
    // {
    //   this->Format = AL_FORMAT_MONO16;
    // }
    // else
    // {
    //   this->Format = AL_FORMAT_STEREO16;
    // }
  }

  OggData::~OggData()
  {
    fclose(file);
    ov_clear(&data);
  }
} // namespace dl