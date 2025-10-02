#!/bin/bash

echo "Building standalone target..."

# Build only the standalone target
cmake --build cmake-build-debug --target Obliterator_Standalone

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Starting standalone app..."

    # Run the standalone app
    ./cmake-build-debug/Obliterator_artefacts/Standalone/Obliterator.app/Contents/MacOS/Obliterator
else
    echo "Build failed!"
    exit 1
fi