#!/bin/bash
# AsioRSPS (#464) C++ Server - Clean Build and Launch
# This script cleans, builds, and launches the C++ server

# Change to script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================"
echo " Protocol #464 C++ Server Launcher"
echo "======================================"
echo "Working directory: $SCRIPT_DIR"

# Step 1: Clean build artifacts
echo ""
echo "[1/4] Cleaning build files..."

if [ -d "build" ]; then
    echo "Removing CMake build directory..."
    rm -rf build
fi

if [ -d "bin" ]; then
    echo "Removing Premake bin directory..."
    rm -rf bin
fi

if [ -d "obj" ]; then
    echo "Removing Premake obj directory..."
    rm -rf obj
fi

rm -f *.sln *.vcxproj *.vcxproj.filters *.vcxproj.user 2>/dev/null

echo "Clean complete!"

# Step 2: Build the project
echo ""
echo "[2/4] Building project..."

mkdir -p build
cd build

echo "Running CMake..."
echo "Configuring with new NPCUpdating files..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake generation failed!"
    read -p "Press Enter to continue..."
    exit 1
fi

echo "Building with make (this will compile NPCUpdating.cpp)..."
cmake --build . --config Release --parallel
if [ $? -ne 0 ]; then
    echo "Build failed!"
    echo ""
    echo "Please check:"
    echo "1. NPCUpdating.cpp and NPCUpdating.h exist in src/"
    echo "2. No compilation errors in the new files"
    read -p "Press Enter to continue..."
    exit 1
fi

cd ..
echo "Build complete!"

# Step 3: Kill any existing processes on port 43594
echo ""
echo "[3/4] Killing processes on port 43594..."

lsof -ti:43594 | xargs kill -9 2>/dev/null || echo "No process found on port 43594"

echo "Port 43594 cleared!"

# Step 4: Launch the server
echo ""
echo "[4/4] Launching server..."

# Create logs directory if it doesn't exist
mkdir -p logs

# Generate log filename with full datetime stamp
LOG_FILE="logs/server_$(date '+%Y-%m-%d_%H-%M-%S').log"

SERVER_EXE=""
if [ -f "build/AsioRSPS-Skeleton" ]; then
    SERVER_EXE="build/AsioRSPS-Skeleton"
elif [ -f "build/Release/AsioRSPS-Skeleton" ]; then
    SERVER_EXE="build/Release/AsioRSPS-Skeleton"
elif [ -f "build/Debug/AsioRSPS-Skeleton" ]; then
    SERVER_EXE="build/Debug/AsioRSPS-Skeleton"
elif [ -f "bin/Release-macosx-x86_64/AsioRSPS-Skeleton/AsioRSPS-Skeleton" ]; then
    SERVER_EXE="bin/Release-macosx-x86_64/AsioRSPS-Skeleton/AsioRSPS-Skeleton"
elif [ -f "bin/Release-macosx-arm64/AsioRSPS-Skeleton/AsioRSPS-Skeleton" ]; then
    SERVER_EXE="bin/Release-macosx-arm64/AsioRSPS-Skeleton/AsioRSPS-Skeleton"
fi

if [ -n "$SERVER_EXE" ]; then
    echo "Starting server: $SERVER_EXE"
    echo "Logging to: $LOG_FILE"
    echo "Server is now running on port 43594..."
    echo "Press Ctrl+C to stop the server."
    echo ""
    echo "=== Server started at $(date '+%Y-%m-%d %H:%M:%S') ===" | tee "$LOG_FILE"
    "./$SERVER_EXE" 2>&1 | while IFS= read -r line; do
        echo "[$(date '+%Y-%m-%d %H:%M:%S.%3N')] $line" | tee -a "$LOG_FILE"
    done
    EXIT_CODE=${PIPESTATUS[0]}
    echo "=== Server stopped at $(date '+%Y-%m-%d %H:%M:%S') with exit code $EXIT_CODE ===" | tee -a "$LOG_FILE"
    
    if [ $EXIT_CODE -ne 0 ]; then
        echo ""
        echo "Server crashed! Check $LOG_FILE for details"
        tail -50 "$LOG_FILE"
    fi
else
    echo "Error: Could not find server executable!"
    echo "Checked locations:"
    echo "  - build/AsioRSPS-Skeleton"
    echo "  - build/Release/AsioRSPS-Skeleton"
    echo "  - build/Debug/AsioRSPS-Skeleton"
    echo "  - bin/Release-macosx-x86_64/AsioRSPS-Skeleton/AsioRSPS-Skeleton"
    echo "  - bin/Release-macosx-arm64/AsioRSPS-Skeleton/AsioRSPS-Skeleton"
    read -p "Press Enter to continue..."
fi
