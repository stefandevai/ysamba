#include "sdl2_webgpu.h"

#ifdef __APPLE__
#include <SDL_syswm.h>
#else
#include <SDL2/SDL_syswm.h>
#endif

#if defined(SDL_VIDEO_DRIVER_COCOA)
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window* window) {
    SDL_SysWMinfo windowWMInfo;
    SDL_VERSION(&windowWMInfo.version);
    SDL_GetWindowWMInfo(window, &windowWMInfo);

#if defined(SDL_VIDEO_DRIVER_COCOA)
    {
        id metal_layer = NULL;
        NSWindow* ns_window = windowWMInfo.info.cocoa.window;
        [ns_window.contentView setWantsLayer : YES] ;
        metal_layer = [CAMetalLayer layer];
        [ns_window.contentView setLayer : metal_layer] ;
        return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
            .label = NULL,
                .nextInChain =
                (const WGPUChainedStruct*)&(
                    WGPUSurfaceDescriptorFromMetalLayer) {
                .chain =
                    (WGPUChainedStruct){
                        .next = NULL,
                        .sType = WGPUSType_SurfaceDescriptorFromMetalLayer,
                },
                .layer = metal_layer,
            },
        });
    }
#elif defined(SDL_VIDEO_DRIVER_X11)
    {
        Display* x11_display = windowWMInfo.info.x11.display;
        Window x11_window = windowWMInfo.info.x11.window;
        return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
            .label = NULL,
                .nextInChain =
                (const WGPUChainedStruct*)&(
                    WGPUSurfaceDescriptorFromXlibWindow) {
                .chain =
                    (WGPUChainedStruct){
                        .next = NULL,
                        .sType = WGPUSType_SurfaceDescriptorFromXlibWindow,
                },
                .display = x11_display,
                .window = x11_window,
            },
        });
    }
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
    {
        struct wl_display* wayland_display = windowWMInfo.info.wl.display;
        struct wl_surface* wayland_surface = windowWMInfo.info.wl.display;
        return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
            .label = NULL,
                .nextInChain =
                (const WGPUChainedStruct*)&(
                    WGPUSurfaceDescriptorFromWaylandSurface) {
                .chain =
                    (WGPUChainedStruct){
                        .next = NULL,
                        .sType =
                            WGPUSType_SurfaceDescriptorFromWaylandSurface,
                        },
                        .display = wayland_display,
                        .surface = wayland_surface,
                },
        });
  }
#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
    {
        HWND hwnd = windowWMInfo.info.win.window;
        HINSTANCE hinstance = GetModuleHandle(NULL);
        return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
            .label = NULL,
                .nextInChain =
                (const WGPUChainedStruct*)&(
                    WGPUSurfaceDescriptorFromWindowsHWND) {
                .chain =
                    (WGPUChainedStruct){
                        .next = NULL,
                        .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
            },
            .hinstance = hinstance,
            .hwnd = hwnd,
        },
    });
  }
#else
#error "Unsupported WGPU_TARGET"
#endif
}

