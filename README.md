# C++ AsioRSPS

This is a modern C++ skeleton for a RuneScape Private Server. It uses [ASIO](https://think-async.com/) for networking and provides only the minimal set of features needed to support:

- Basic walking
- Multiplayer functionality

The architecture is inspired by [RuneSource](https://github.com/dozmus/runesource).

## Features

- ASIO-based networking (header-only, no Boost required) **(included in vendor/)**
- Player connection handling and movement processing
- Multiplayer synchronization
- Logging via [spdlog](https://github.com/gabime/spdlog) **(included in vendor/)**
- Lightweight and extensible design

## Build Options

This project supports both **Premake5** and **CMake** for generating build files.

### Using Premake5

To generate Visual Studio 2022 project files, run the included `GenerateProjects.bat` script in the project root.

> **Note:** The Premake5 binary is **not included** in the repository.  
> You must [download it from the official site](https://premake.github.io/download) and place it in `vendor/bin/premake/` or update the script path accordingly.

### Using CMake

Cross-platform support via CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
