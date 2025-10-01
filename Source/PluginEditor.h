#pragma once

#include "PluginProcessor.h"
#include "DistortionLookAndFeel.h"
#include "OscilloscopeComponent.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    // Background SVG
    std::unique_ptr<juce::Drawable> backgroundSVG;

    // Custom font
    juce::Font sankofaFont;

    // Custom LookAndFeel for knobs
    DistortionLookAndFeel customLookAndFeel;

    // Oscilloscope
    OscilloscopeComponent oscilloscope;

    // UI Components
    juce::Slider driveSlider;
    juce::Label driveLabel;
    juce::Label driveValueLabel;
    juce::Slider asymmetrySlider;
    juce::Label asymmetryLabel;
    juce::Label asymmetryValueLabel;
    juce::Slider subOctaveSlider;
    juce::Label subOctaveLabel;
    juce::Label subOctaveValueLabel;
    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;
    juce::Label dryWetValueLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> asymmetryAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subOctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
