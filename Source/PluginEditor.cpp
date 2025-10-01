#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
        AudioPluginAudioProcessor& p) :
    AudioProcessorEditor(&p), processorRef(p)
{
    // Configure drive slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setRange(1.0, 1000.0, 0.01);
    driveSlider.setValue(1.0);
    driveSlider.setLookAndFeel(
            &customLookAndFeel); // Apply custom knob graphics
    addAndMakeVisible(driveSlider);

    // Configure label
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setFont(juce::Font(14.0f)); // Larger font size
    addAndMakeVisible(driveLabel);

    // Configure asymmetry slider
    asymmetrySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    asymmetrySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    asymmetrySlider.setRange(-1.0, 1.0, 0.01);
    asymmetrySlider.setValue(0.0);
    asymmetrySlider.setLookAndFeel(
            &customLookAndFeel); // Apply custom knob graphics
    addAndMakeVisible(asymmetrySlider);

    // Configure asymmetry label
    asymmetryLabel.setText("Asymmetry", juce::dontSendNotification);
    asymmetryLabel.setJustificationType(juce::Justification::centred);
    asymmetryLabel.setFont(juce::Font(14.0f)); // Larger font size
    addAndMakeVisible(asymmetryLabel);

    // Configure sub-octave slider
    subOctaveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    subOctaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subOctaveSlider.setRange(0.0, 1.0, 0.01);
    subOctaveSlider.setValue(0.0);
    subOctaveSlider.setLookAndFeel(
            &customLookAndFeel); // Apply custom knob graphics
    addAndMakeVisible(subOctaveSlider);

    // Configure sub-octave label
    subOctaveLabel.setText("Sub Octave", juce::dontSendNotification);
    subOctaveLabel.setJustificationType(juce::Justification::centred);
    subOctaveLabel.setFont(juce::Font(14.0f)); // Larger font size
    addAndMakeVisible(subOctaveLabel);

    // Configure dry/wet slider
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    dryWetSlider.setRange(0.0, 1.0, 0.01);
    dryWetSlider.setValue(1.0); // Default to fully wet (rightmost position)
    dryWetSlider.setLookAndFeel(
            &customLookAndFeel); // Apply custom knob graphics
    addAndMakeVisible(dryWetSlider);

    // Configure dry/wet label
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    dryWetLabel.setFont(juce::Font(14.0f)); // Larger font size
    addAndMakeVisible(dryWetLabel);

    // Configure value labels (positioned below knobs)
    driveValueLabel.setJustificationType(juce::Justification::centred);
    driveValueLabel.setText("1.0", juce::dontSendNotification);
    addAndMakeVisible(driveValueLabel);

    asymmetryValueLabel.setJustificationType(juce::Justification::centred);
    asymmetryValueLabel.setText("0.0", juce::dontSendNotification);
    addAndMakeVisible(asymmetryValueLabel);

    subOctaveValueLabel.setJustificationType(juce::Justification::centred);
    subOctaveValueLabel.setText("0.0", juce::dontSendNotification);
    addAndMakeVisible(subOctaveValueLabel);

    dryWetValueLabel.setJustificationType(juce::Justification::centred);
    dryWetValueLabel.setText("1.0", juce::dontSendNotification);
    addAndMakeVisible(dryWetValueLabel);

    // Attach parameters
    driveAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "drive", driveSlider);
    asymmetryAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "asymmetry", asymmetrySlider);
    subOctaveAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "suboctave", subOctaveSlider);
    dryWetAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.parameters, "drywet", dryWetSlider);

    // Add slider listeners to update value labels
    driveSlider.onValueChange = [this]() {
        driveValueLabel.setText(juce::String(driveSlider.getValue(), 1),
                                juce::dontSendNotification);
    };
    asymmetrySlider.onValueChange = [this]() {
        asymmetryValueLabel.setText(juce::String(asymmetrySlider.getValue(), 2),
                                    juce::dontSendNotification);
    };
    subOctaveSlider.onValueChange = [this]() {
        subOctaveValueLabel.setText(juce::String(subOctaveSlider.getValue(), 2),
                                    juce::dontSendNotification);
    };
    dryWetSlider.onValueChange = [this]() {
        dryWetValueLabel.setText(juce::String(dryWetSlider.getValue(), 2),
                                 juce::dontSendNotification);
    };

    // Load background SVG
    backgroundSVG = juce::Drawable::createFromImageData(
            BinaryData::background_svg, BinaryData::background_svgSize);

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
    // Draw SVG background if loaded, otherwise fallback to solid color
    if (backgroundSVG)
    {
        backgroundSVG->drawWithin(g, getLocalBounds().toFloat(),
                                  juce::RectanglePlacement::stretchToFit, 1.0f);
    }
    else
    {
        // Fallback to solid background if SVG fails to load
        g.fillAll(getLookAndFeel().findColour(
                juce::ResizableWindow::backgroundColourId));
    }

    // Optional: Draw title text on top of SVG if needed
    // g.setColour(juce::Colours::white);
    // g.setFont(15.0f);
    // auto titleBounds = getLocalBounds().removeFromTop(40);
    // g.drawFittedText("Distortion Plugin", titleBounds,
    //                  juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Reserve space for title at top (matches the 40px from paint())
    auto titleArea = bounds.removeFromTop(40);

    // Create main slider area
    auto sliderArea = bounds.reduced(50);

    // Split area horizontally for four sliders
    auto driveArea = sliderArea.removeFromLeft(sliderArea.getWidth() / 4);
    auto asymmetryArea = sliderArea.removeFromLeft(sliderArea.getWidth() / 3);
    auto subOctaveArea = sliderArea.removeFromLeft(sliderArea.getWidth() / 2);
    auto dryWetArea = sliderArea;

    // Add some spacing between sliders
    driveArea = driveArea.reduced(10);
    asymmetryArea = asymmetryArea.reduced(10);
    subOctaveArea = subOctaveArea.reduced(10);
    dryWetArea = dryWetArea.reduced(10);

    // Don't reserve space - position value labels right against knobs
    auto valueHeight = 15;

    // Use the remaining space for knobs
    auto driveKnobArea = driveArea;
    auto asymmetryKnobArea = asymmetryArea;
    auto subOctaveKnobArea = subOctaveArea;
    auto dryWetKnobArea = dryWetArea;

    // Position knobs first
    driveSlider.setBounds(driveKnobArea);
    asymmetrySlider.setBounds(asymmetryKnobArea);
    subOctaveSlider.setBounds(subOctaveKnobArea);
    dryWetSlider.setBounds(dryWetKnobArea);

    // Position labels just above the arc (calculated based on knob bounds)
    auto labelHeight = 15;
    auto arcOffset = driveKnobArea.getHeight() *
                     0.15f; // Distance from knob center to arc top

    auto driveLabelY =
            driveKnobArea.getY() + arcOffset + 15; // Much closer to knob center
    auto asymmetryLabelY = asymmetryKnobArea.getY() + arcOffset + 15;
    auto subOctaveLabelY = subOctaveKnobArea.getY() + arcOffset + 15;
    auto dryWetLabelY = dryWetKnobArea.getY() + arcOffset + 15;

    driveLabel.setBounds(driveKnobArea.getX(), driveLabelY,
                         driveKnobArea.getWidth(), labelHeight);
    asymmetryLabel.setBounds(asymmetryKnobArea.getX(), asymmetryLabelY,
                             asymmetryKnobArea.getWidth(), labelHeight);
    subOctaveLabel.setBounds(subOctaveKnobArea.getX(), subOctaveLabelY,
                             subOctaveKnobArea.getWidth(), labelHeight);
    dryWetLabel.setBounds(dryWetKnobArea.getX(), dryWetLabelY,
                          dryWetKnobArea.getWidth(), labelHeight);

    // Position value labels right against the bottom of knobs
    auto knobCenterY = driveKnobArea.getCentreY();
    auto knobRadius =
            driveKnobArea.getWidth() * 0.3f; // Approximate knob radius
    auto valueY =
            knobCenterY + knobRadius + 10; // Slightly lower below knob edge

    driveValueLabel.setBounds(driveKnobArea.getX(), valueY,
                              driveKnobArea.getWidth(), valueHeight);
    asymmetryValueLabel.setBounds(asymmetryKnobArea.getX(), valueY,
                                  asymmetryKnobArea.getWidth(), valueHeight);
    subOctaveValueLabel.setBounds(subOctaveKnobArea.getX(), valueY,
                                  subOctaveKnobArea.getWidth(), valueHeight);
    dryWetValueLabel.setBounds(dryWetKnobArea.getX(), valueY,
                               dryWetKnobArea.getWidth(), valueHeight);
}
