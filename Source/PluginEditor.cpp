#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
        AudioPluginAudioProcessor& p) :
    AudioProcessorEditor(&p), processorRef(p)
{
    // Configure drive slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    driveSlider.setRange(1.0, 1000.0, 0.01);
    driveSlider.setValue(1.0);
    addAndMakeVisible(driveSlider);

    // Configure label
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    driveLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(driveLabel);

    // Configure asymmetry slider
    asymmetrySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    asymmetrySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    asymmetrySlider.setRange(-1.0, 1.0, 0.01);
    asymmetrySlider.setValue(0.0);
    addAndMakeVisible(asymmetrySlider);

    // Configure asymmetry label
    asymmetryLabel.setText("Asymmetry", juce::dontSendNotification);
    asymmetryLabel.attachToComponent(&asymmetrySlider, false);
    asymmetryLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(asymmetryLabel);

    // Attach parameters
    driveAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "drive", driveSlider);
    asymmetryAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "asymmetry", asymmetrySlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(680, 450);

    // Make the plugin window resizable
    setResizable(true, true);

    // Fix aspect ratio to maintain proportions
    getConstrainer()->setFixedAspectRatio(680.0 / 450.0);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(getLookAndFeel().findColour(
            juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);

    g.setFont(15.0f);

    auto titleBounds = getLocalBounds().removeFromTop(40);
    g.drawFittedText("Distortion Plugin.f", titleBounds,
                     juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Reserve space for title at top (matches the 40px from paint())
    auto titleArea = bounds.removeFromTop(40);

    // Create main slider area
    auto sliderArea = bounds.reduced(50);

    // Split area horizontally for two sliders
    auto driveArea = sliderArea.removeFromLeft(sliderArea.getWidth() / 2);
    auto asymmetryArea = sliderArea;

    // Add some spacing between sliders
    driveArea = driveArea.reduced(10);
    asymmetryArea = asymmetryArea.reduced(10);

    driveSlider.setBounds(driveArea);
    asymmetrySlider.setBounds(asymmetryArea);
}
