#pragma once

#ifdef __APPLE__
#include <SDL_video.h>
#else
#include <SDL2/SDL_video.h>
#endif
#include <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window* window);

#ifdef __cplusplus
}
#endif
