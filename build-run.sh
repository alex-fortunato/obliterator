#!/bin/bash

echo "Building standalone target..."

# Build only the standalone target
cmake --build cmake-build-debug --target PluginName_Standalone

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Starting standalone app..."

    # Run the standalone app
    ./cmake-build-debug/PluginName_artefacts/Standalone/PluginName.app/Contents/MacOS/PluginName
else
    echo "Build failed!"
    exit 1
fi