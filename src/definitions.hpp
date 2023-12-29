#define DL_BUILD_DEBUG_TOOLS

#if defined(__APPLE__) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__linux__)
#define DL_HAS_SUPPORTED_PLATFORM_FOR_USAGE
#endif
