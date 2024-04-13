#include "./stream.hpp"

#include <AL/alc.h>
#include <spdlog/spdlog.h>

// #include <iostream>

// namespace
// {
// std::string error_to_string (int code)
// {
//   switch (code)
//   {
//     case OV_EREAD:
//       return "Read from media.";
//     case OV_ENOTVORBIS:
//       return "Not vorbis data.";
//     case OV_EVERSION:
//       return "Vorbis version mismatch";
//     case OV_EBADHEADER:
//       return "Invalid vorbis header.";
//     case OV_EFAULT:
//       return "Internal logic fault";
//     default:
//       return "Unknown Ogg error.";
//   }
// }
// } // namespace

namespace dl::audio
{
Stream::Stream(const std::string& filepath)
{
  ogg = std::make_unique<OggData>(filepath);

  ALenum audio_format;

  if (ogg->metadata->channels == 1)
  {
    audio_format = AL_FORMAT_MONO16;
  }
  else
  {
    audio_format = AL_FORMAT_STEREO16;
  }

  // alGenBuffers (STREAM_BUFFERS, buffers);

  // for(std::size_t i = 0; i < STREAM_BUFFERS; ++i)
  // {
  //   alBufferData(buffers[i], audio_format, , BUFFER_SIZE, ogg->metadata->rate)
  //   alCall(alBufferData, buffers[i], format, &soundData[i * BUFFER_SIZE], BUFFER_SIZE, sampleRate);
  // }

  // if (!(file = fopen(filepath.c_str(), "rb")))
  // {
  //   spdlog::critical("Failed to open ogg file");
  //   return;
  // }
  //
  // int result = ov_open(file, &file_data, NULL, 0);
  //
  // if (result < 0)
  // {
  //   spdlog::critical("Failed to open ogg stream: {}", error_to_string(result));
  //   fclose (file);
  //   return;
  // }
  //
  // metadata = ov_info(&file_data, -1);
  // size = ov_pcm_total(&file_data, -1) * metadata->channels * 2;

  // buffer = (char*)malloc(size);
  //
  // size_t bytes_read = -1, offset = 0;
  // int sel = 0;
  //
  // while (bytes_read != 0)
  // {
  //   bytes_read = ov_read(&file_data, buffer + offset, 4096, 0, 2, 1, &sel);
  //
  //   if(bytes_read < 0)
  //   {
  //     spdlog::critical("Error reading OGG file");
  //     return;
  //   }
  //
  //   offset += bytes_read;
  // }
}

Stream::~Stream()
{
  // if (size > 0)
  // {
  //   free(buffer);
  // }
  //
  // fclose(file);
  // ov_clear(&file_data);
}
}  // namespace dl::audio
