#include "./ogg_data.hpp"

#include <spdlog/spdlog.h>

#include "audio/utils.hpp"

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
    utils::check_ogg_error(result);
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
