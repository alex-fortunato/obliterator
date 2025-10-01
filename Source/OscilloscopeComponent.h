#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class OscilloscopeComponent : public juce::Component, private juce::Timer
{
public:
    OscilloscopeComponent();
    ~OscilloscopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Called from audio thread to push new samples
    void pushBuffer(const float* data, int numSamples);

private:
    void timerCallback() override;

    static constexpr int bufferSize = 512;
    juce::AudioBuffer<float> audioBuffer;
    juce::AbstractFifo abstractFifo { bufferSize };

    std::vector<float> displayBuffer;
    juce::CriticalSection pathCreationLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopeComponent)
};
