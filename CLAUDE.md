# Distortion Plugin - JUCE Project

## Project Overview
This is a JUCE audio plugin that implements tanh-based distortion/saturation. Built using JUCE 8.0.6 with CMake.

## Current State
- **Plugin Type:** Effect plugin (not synth)
- **Formats:** AU, VST3, AAX, Standalone
- **Parameter:** Single "Drive" knob (1.0 to 25.0)
- **Algorithm:** Hyperbolic tangent (tanh) saturation
- **UI:** 680x450 resizable window with fixed aspect ratio

## Key Implementation Details

### Core Algorithm
```cpp
// Drive = 1.0: Unity gain passthrough
// Drive > 1.0: tanh(drive * input) saturation
if (currentDrive <= 1.0f)
    channelData[sample] = inputSample; // No processing
else
    channelData[sample] = std::tanh(currentDrive * inputSample);
```

### Parameter Management
- Uses JUCE's AudioProcessorValueTreeState (APVTS)
- Parameter ID: "drive"
- Range: 1.0 to 25.0 (with 0.3 skew factor)
- Default: 1.0 (leftmost position = no effect)

### UI Components
- Single rotary slider with text box below
- SliderAttachment connects UI to parameter
- Title "Distortion Plugin.e" at top
- Resizable with fixed aspect ratio (680:450)

## Design Decisions Made

### Gain Behavior
- **No gain compensation:** Pure tanh behavior preferred
- **No attenuation:** Plugin never reduces signal below unity gain
- **Drive = 1.0:** Perfect passthrough (no processing applied)
- **Higher drive:** More saturation + natural level increase

### Parameter Range Evolution
- Started: 0.1 to 10.0 (caused attenuation issues)
- Final: 1.0 to 25.0 (unity gain at minimum, extreme saturation available)

## Build System
- CMake-based build
- JUCE fetched automatically from GitHub
- Uses cmake-build-debug directory for builds
- Plugin name: "PluginName" (should be updated to "DistortionPlugin")

## File Structure
```
Source/
├── PluginProcessor.h/cpp    # Audio processing & parameters
└── PluginEditor.h/cpp       # UI components & layout
CMakeLists.txt               # Build configuration
```

## Development Notes
- Tested in Bitwig Studio
- Uses Bitwig's Commander (Ctrl+Enter) for plugin rescanning
- tanh() available through JuceHeader.h (includes <cmath> internally)
- Real-time safe processing (no memory allocation in audio thread)

## Next Possible Features
- Mix/Wet-Dry control
- Output level control
- Different distortion algorithms (cubic, asymmetric, etc.)
- Oversampling for higher quality
- Visual feedback (spectrum analyzer, oscilloscope)

## Current Issues/TODOs
- Plugin name still shows as "PluginName" instead of "DistortionPlugin"
- Could benefit from additional visual polish
- Consider adding tooltips for parameters