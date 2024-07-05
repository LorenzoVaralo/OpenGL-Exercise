#!/bin/sh

BUILD_DIR="build"

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
fi

# Run CMake in the build directory
cd "$BUILD_DIR"
cmake ..
make
