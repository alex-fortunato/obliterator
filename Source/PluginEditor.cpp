#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
        AudioPluginAudioProcessor& p) :
    AudioProcessorEditor(&p), processorRef(p)
{
    // Load custom font
    sankofaFont = juce::Font(juce::Typeface::createSystemTypefaceFor(
        BinaryData::SankofaDisplayRegular_ttf,
        BinaryData::SankofaDisplayRegular_ttfSize));

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
    driveLabel.setFont(sankofaFont.withHeight(14.0f));
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
    asymmetryLabel.setFont(sankofaFont.withHeight(14.0f));
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
    subOctaveLabel.setText("Sub", juce::dontSendNotification);
    subOctaveLabel.setJustificationType(juce::Justification::centred);
    subOctaveLabel.setFont(sankofaFont.withHeight(14.0f));
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
    dryWetLabel.setFont(sankofaFont.withHeight(14.0f));
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

    // Setup oscilloscope
    addAndMakeVisible(oscilloscope);
    processorRef.setOscilloscopeComponent(&oscilloscope);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(680, 450);

    // Make the plugin window resizable
    setResizable(false, false);

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

    // Position oscilloscope in top-right area
    int oscWidth = 280;
    int oscHeight = 183;
    int oscX = bounds.getWidth() - 206 -
               (oscWidth / 2); // Centered with drive knob
    int oscY = 41; // 41px from top
    oscilloscope.setBounds(oscX, oscY, oscWidth, oscHeight);

    // Define knob sizes (including arcs)
    const int driveKnobSize = 115; // Arc diameter for drive
    const int smallKnobSize = 68; // Arc diameter for other knobs

    // Position drive knob: bottom-right area
    // Center at 206px from right edge, bottom 33px from absolute bottom
    int driveCenterX = bounds.getWidth() - 206;
    int driveCenterY = bounds.getHeight() - 33 - (driveKnobSize / 2);
    auto driveArea =
            juce::Rectangle<int>(driveKnobSize, driveKnobSize)
                    .withCentre(juce::Point<int>(driveCenterX, driveCenterY));

    // Position asymmetry knob: inline with drive, to the left
    int asymmetryCenterX = driveCenterX - 150; // 150px to the left of drive
    int asymmetryCenterY = driveCenterY;
    auto asymmetryArea = juce::Rectangle<int>(smallKnobSize, smallKnobSize)
                                 .withCentre(juce::Point<int>(
                                         asymmetryCenterX, asymmetryCenterY));

    // Position sub-octave knob: inline with drive, to the right
    int subOctaveCenterX = driveCenterX + 150; // 150px to the right of drive
    int subOctaveCenterY = driveCenterY;
    auto subOctaveArea = juce::Rectangle<int>(smallKnobSize, smallKnobSize)
                                 .withCentre(juce::Point<int>(
                                         subOctaveCenterX, subOctaveCenterY));

    // Position dry/wet knob: top-left area
    int dryWetCenterX = 80; // 80px from left edge
    int dryWetCenterY = 80; // 80px from top
    auto dryWetArea =
            juce::Rectangle<int>(smallKnobSize, smallKnobSize)
                    .withCentre(juce::Point<int>(dryWetCenterX, dryWetCenterY));

    // Set slider bounds
    driveSlider.setBounds(driveArea);
    asymmetrySlider.setBounds(asymmetryArea);
    subOctaveSlider.setBounds(subOctaveArea);
    dryWetSlider.setBounds(dryWetArea);

    // Position labels above each knob
    auto labelHeight = 15;
    auto labelWidth = 80;

    driveLabel.setBounds(driveCenterX - labelWidth / 2,
                         driveArea.getY() - labelHeight - 5, labelWidth,
                         labelHeight);
    asymmetryLabel.setBounds(asymmetryCenterX - labelWidth / 2,
                             asymmetryArea.getY() - labelHeight - 5, labelWidth,
                             labelHeight);
    subOctaveLabel.setBounds(subOctaveCenterX - labelWidth / 2,
                             subOctaveArea.getY() - labelHeight - 5, labelWidth,
                             labelHeight);
    dryWetLabel.setBounds(dryWetCenterX - labelWidth / 2,
                          dryWetArea.getY() - labelHeight - 5, labelWidth,
                          labelHeight);

    // Position value labels below each knob
    auto valueHeight = 15;

    driveValueLabel.setBounds(driveCenterX - labelWidth / 2,
                              driveArea.getBottom() + 5, labelWidth,
                              valueHeight);
    asymmetryValueLabel.setBounds(asymmetryCenterX - labelWidth / 2,
                                  asymmetryArea.getBottom() + 5, labelWidth,
                                  valueHeight);
    subOctaveValueLabel.setBounds(subOctaveCenterX - labelWidth / 2,
                                  subOctaveArea.getBottom() + 5, labelWidth,
                                  valueHeight);
    dryWetValueLabel.setBounds(dryWetCenterX - labelWidth / 2,
                               dryWetArea.getBottom() + 5, labelWidth,
                               valueHeight);
}
