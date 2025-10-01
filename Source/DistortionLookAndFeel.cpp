#include "DistortionLookAndFeel.h"
#include "BinaryData.h"

DistortionLookAndFeel::DistortionLookAndFeel()
{
    // Load the knob SVG from binary data
    knobSVG = juce::Drawable::createFromImageData(
        BinaryData::knob1_svg, BinaryData::knob1_svgSize);
}

void DistortionLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    if (!knobSVG)
    {
        // Fallback to default slider if SVG fails to load
        juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos,
                                              rotaryStartAngle, rotaryEndAngle, slider);
        return;
    }

    // Calculate the rotation angle based on slider position
    auto currentAngle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

    // Create the bounds for the knob (smaller to leave room for arc around it)
    auto knobBounds = juce::Rectangle<float>(x, y, width, height).reduced(20.0f);

    // Calculate center point for rotation
    auto centerX = knobBounds.getCentreX();
    auto centerY = knobBounds.getCentreY();

    // Create rotation transform around the center
    auto transform = juce::AffineTransform::rotation(currentAngle, centerX, centerY);

    // Apply the transform and draw the knob
    g.saveState();
    g.addTransform(transform);

    // Draw the SVG knob centered in the bounds
    knobSVG->drawWithin(g, knobBounds, juce::RectanglePlacement::centred, 1.0f);

    g.restoreState();

    // Draw progress arc around the knob
    // Use knob center for arc positioning
    auto arcCenterX = centerX;
    auto arcCenterY = centerY;

    // Make arc diameter larger than knob to circle around it
    auto arcRadius = knobBounds.getWidth() * 0.6f; // Larger than knob for circling effect
    auto arcThickness = 2.0f;
    auto arcStartAngle = -2.356f; // -135 degrees in radians
    auto arcEndAngle = 2.356f;    // +135 degrees in radians
    auto arcCurrentAngle = arcStartAngle + (sliderPos * (arcEndAngle - arcStartAngle));

    // Draw grey background arc (full range)
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(arcCenterX, arcCenterY, arcRadius, arcRadius,
                               0.0f, arcStartAngle, arcEndAngle, true);
    g.setColour(juce::Colours::grey);
    g.strokePath(backgroundArc, juce::PathStrokeType(arcThickness));

    // Check if this is a bipolar slider (asymmetry: -1.0 to 1.0)
    bool isBipolar = (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0);

    if (isBipolar)
    {
        // Bipolar behavior: fill from center outward
        auto centerAngle = (arcStartAngle + arcEndAngle) * 0.5f; // Center position (0 degrees)

        if (std::abs(sliderPos - 0.5f) > 0.001f) // Only draw if not at center
        {
            juce::Path filledArc;
            if (sliderPos > 0.5f) // Right side (positive values)
            {
                filledArc.addCentredArc(arcCenterX, arcCenterY, arcRadius, arcRadius,
                                       0.0f, centerAngle, arcCurrentAngle, true);
            }
            else // Left side (negative values)
            {
                filledArc.addCentredArc(arcCenterX, arcCenterY, arcRadius, arcRadius,
                                       0.0f, arcCurrentAngle, centerAngle, true);
            }
            g.setColour(juce::Colours::red);
            g.strokePath(filledArc, juce::PathStrokeType(arcThickness));
        }
    }
    else
    {
        // Unipolar behavior: fill from left to current position
        if (sliderPos > 0.0f) // Only draw if there's something to fill
        {
            juce::Path filledArc;
            filledArc.addCentredArc(arcCenterX, arcCenterY, arcRadius, arcRadius,
                                   0.0f, arcStartAngle, arcCurrentAngle, true);
            g.setColour(juce::Colours::red);
            g.strokePath(filledArc, juce::PathStrokeType(arcThickness));
        }
    }
}