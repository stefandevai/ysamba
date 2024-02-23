cmake_minimum_required (VERSION 3.13...3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()


project(
    dialectics
    LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")  # Keep all runtime files in one directory.

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.h
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Ensure the C++17 standard is available.
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Enforce UTF-8 encoding on MSVC.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)  # Defines M_PI
endif()

# Enable warnings recommended for new projects.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WD4244)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3)
endif()

# Set up WebGPU
include(FetchContent)

FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  GIT_TAG        wgpu-static
)

FetchContent_MakeAvailable(webgpu)

# Set up dependencies under lib directory
set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
add_subdirectory("lib/FastNoise2")
add_subdirectory("lib/i18n_keyval")

# Force an optimized build of FastNoise2
if (MSVC)
else()
    target_compile_options(FastNoise PRIVATE -O3)
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(libtcod CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "./lib/gal/include"
    "./src"
)

SET(
    DL_LIBRARIES
    webgpu
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    libtcod::libtcod
    cereal::cereal
    EnTT::EnTT
    nlohmann_json::nlohmann_json
    freetype
    fmt::fmt
    spdlog::spdlog
    imgui::imgui
    implot::implot
    FastNoise
    i18n_keyval::i18n_keyval
)

# Allow objective-c for SDL2 C file on Apple platforms
if(APPLE)
  set_source_files_properties("src/graphics/renderer/sdl2_webgpu.c" PROPERTIES COMPILE_FLAGS "-x objective-c" LANGUAGE C)
  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${DL_LIBRARIES}
    "-framework QuartzCore"
    "-framework Cocoa"
    "-framework Metal"
  )
else()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${DL_LIBRARIES}
    )
endif()
