cmake_minimum_required(VERSION 3.11)

project(AsioRSPS)

set(CMAKE_CXX_STANDARD 20)

add_executable(AsioRSPS-Skeleton)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
target_sources(AsioRSPS-Skeleton PRIVATE ${SOURCES})

target_include_directories(AsioRSPS-Skeleton PRIVATE
    vendor/asio/include
    vendor/spdlog/include
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AsioRSPS-Skeleton)