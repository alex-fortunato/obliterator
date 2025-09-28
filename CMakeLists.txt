cmake_minimum_required(VERSION 3.22)
project(PluginName VERSION 1.1.0)
set(CMAKE_CXX_STANDARD 17)

# Generate compile_commands.json for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Set persistent directory for dependencies outside build folder
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/libs")

# Fetch JUCE 8.x at configure time so users don't need it pre-installed.
# If you prefer a local JUCE, set JUCE_SOURCE_DIR to skip fetch.
if (DEFINED JUCE_SOURCE_DIR)
    message(STATUS "Using local JUCE at ${JUCE_SOURCE_DIR}")
    add_subdirectory(${JUCE_SOURCE_DIR} JUCE)
else()
    message(STATUS "Fetching JUCE from GitHub (8.0.6)...")
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.6
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(juce)
endif()

# Set up your plugin
juce_add_plugin(PluginName
        VERSION 1.1.0
        FORMATS AU VST3 AAX Standalone
        PRODUCT_NAME "PluginName"
        COMPANY_NAME AlexFortunatoMusic

        IS_SYNTH FALSE                       # Is this a synth or an effect?
        NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
        NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
        IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
        COPY_PLUGIN_AFTER_BUILD TRUE         # Should the plugin be installed to a default location after building?
        PLUGIN_MANUFACTURER_CODE Fort        # A four-character manufacturer id with at least one upper-case character
        PLUGIN_CODE Xxxx                    # A unique four-character plugin id with exactly one upper-case character
        # RESOURCES
        #     resources/background.png
)

# Generate JuceHeader.h
juce_generate_juce_header(PluginName)

# Add source files
target_sources(PluginName PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        # Add other source files here
)

# Link to JUCE modules
target_link_libraries(PluginName PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        # Add other modules as needed
)

# Add include directories for source and shared headers
target_include_directories(PluginName PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        #${CMAKE_BINARY_DIR}/cmake-build-debug/AudioSender_artefacts/JuceLibraryCode
        #${CMAKE_CURRENT_SOURCE_DIR}/libs/juce/modules
        # "/Users/alexanderfortunato/Development/JUCE/Shared Headers"
)

# These definitions are recommended by JUCE.
target_compile_definitions(PluginName
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)
