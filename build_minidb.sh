#!/bin/bash
set -e

echo "Building MiniDB..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Compile
make -j$(nproc)

# Make executable (Linux/macOS)
chmod +x minidb

echo "Build complete! Executable at: $(pwd)/minidb"