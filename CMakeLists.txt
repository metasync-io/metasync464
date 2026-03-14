cmake_minimum_required(VERSION 3.11)
project(AsioRSPS LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json (useful for debugging include paths / clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Sources ----
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# zlib sources
file(GLOB ZLIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib/*.c"
)
list(FILTER ZLIB_SOURCES EXCLUDE REGEX ".*/(example|minigzip|gz).*")

# bzip2 sources (explicit list is safest)
set(BZIP2_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/blocksort.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/huffman.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/crctable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/randtable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/compress.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/decompress.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2/bzlib.c"
)

# ---- Target ----
add_executable(AsioRSPS-Skeleton
    ${SOURCES}
    ${ZLIB_SOURCES}
    ${BZIP2_SOURCES}
)

# IMPORTANT: include src so `#include "epch.h"` works from src/Items/*.cpp
target_include_directories(AsioRSPS-Skeleton PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/asio/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bzip2"
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT AsioRSPS-Skeleton)