#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();


    juce::AudioParameterFloat *driveParameter;
    juce::AudioParameterFloat *asymmetryParameter;
    juce::AudioParameterFloat *subOctaveParameter;
    juce::AudioParameterFloat *dryWetParameter;
    // DSP State:
    float currentDrive = 1.0f;
    float currentAsymmetry = 0.0f;
    float currentSubOctave = 0.0f;
    float currentDryWet = 1.0f;

    // Octave divider state (per channel)
    struct OctaveDividerState {
        bool lastPositive = false;
        bool flipFlop = false;
        float subOscillator = 0.0f;
        float lowpassZ1 = 0.0f; // For smoothing the sub-octave
    };
    OctaveDividerState octaveState[2]; // Left and right channel
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
