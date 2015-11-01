#ifndef __CONFIG_H__
#define __CONFIG_H__
// config type file for define general variables
// such as width, height, just normal config stuff

// SCREEN RESOLUTION
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// RENDERER
#define WINDOWS_RENDER_TYPE BGFX_RENDERER_TYPE_DIRECT3D12
#define DEFAULT_RENDER_TYPE BGFX_RENDERER_TYPE_OPENGL
#ifdef __WIN32
  #define RENDER_TYPE WINDOWS_RENDER_TYPE
#elif defined(_MSC_VER)
  #define RENDER_TYPE WINDOWS_RENDER_TYPE
#else
  #define RENDER_TYPE DEFAULT_RENDER_TYPE
#endif

// RESET PARAMS
#define RESET_PARAMS BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY
#endif
