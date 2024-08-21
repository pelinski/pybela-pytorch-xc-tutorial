#!/bin/bash

# Check if project name is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <project_name>"
  exit 1
fi

PROJECT_NAME=$1
PROJECT_DIR="/workspace/example-project"
TOOLCHAIN_FILE="/workspace/Toolchain.cmake"

# Navigate to the project directory
cd $PROJECT_DIR || exit

# Create and navigate to the build directory
mkdir -p build && cd build || exit

# Run cmake with the provided project name
cmake -DPROJECT_NAME=$PROJECT_NAME -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE ../

# Build the project
cmake --build .