# MetaSync #464 - C++ RSPS 

A modern C++ RuneScape Private Server framework targeting **protocol 464**. 

Originally forked from AsioRSPS (#317), this project has been upgraded with requisite scaffolding for full gameplay development.  


## Summary

Uses ASIO for high-performance networking and includes complete packet handling for the #464 protocol.

Pairs with: https://github.com/metasync-io/Paragon-Resizable-HD

## Features

### Core
- **Protocol 464** support (upgraded from 317)
- ASIO-based networking (header-only, no Boost required)
- Player connection handling and movement processing
- Multiplayer synchronization with player/NPC updating
- Logging via [spdlog](https://github.com/gabime/spdlog)
- Update server for cache delivery (JS5)
- BZIP2/ZLIB compression support

### Combat System Support
- Combat triangle (Melee, Ranged, Magic)
- Combat formulas with accuracy and max hit calculations
- Hit queue and projectile handling
- Special attacks support

### Equipment & Items Support
- Equipment slots with stat bonuses
- Inventory management
- Weapon definitions and attack styles

### Skills Support
- Skill system with experience tracking
- Level calculations

### Magic Support
- Spell system with rune requirements
- Spellbook support

### Player Systems
- Player save/load functionality
- Action queue system
- Chat and commands

### Packet Handlers
- Walking and movement
- Interface interactions
- NPC/Player interactions
- Object interactions
- Item operations (wield, switch, drop)

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
