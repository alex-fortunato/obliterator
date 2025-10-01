#include "OscilloscopeComponent.h"

OscilloscopeComponent::OscilloscopeComponent()
{
    audioBuffer.setSize(1, bufferSize);
    audioBuffer.clear();
    displayBuffer.resize(bufferSize, 0.0f);

    // Refresh at 30 fps for smooth animation
    startTimerHz(30);
}

OscilloscopeComponent::~OscilloscopeComponent()
{
    stopTimer();
}

void OscilloscopeComponent::pushBuffer(const float* data, int numSamples)
{
    // Downsample if needed to fit into our buffer
    if (numSamples <= 0) return;

    int start1, size1, start2, size2;
    abstractFifo.prepareToWrite(numSamples, start1, size1, start2, size2);

    int actualWritten = juce::jmin(numSamples, size1 + size2);

    if (size1 > 0)
    {
        int toCopy = juce::jmin(numSamples, size1);
        audioBuffer.copyFrom(0, start1, data, toCopy);
    }
    if (size2 > 0 && numSamples > size1)
    {
        int toCopy = juce::jmin(numSamples - size1, size2);
        audioBuffer.copyFrom(0, start2, data + size1, toCopy);
    }

    abstractFifo.finishedWrite(actualWritten);
}

void OscilloscopeComponent::timerCallback()
{
    int numReady = abstractFifo.getNumReady();

    // Only update if we have some samples (relaxed condition)
    if (numReady >= 64)  // Lower threshold for more frequent updates
    {
        // Read up to bufferSize samples
        int samplesToRead = juce::jmin(numReady, bufferSize);

        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(samplesToRead, start1, size1, start2, size2);

        juce::ScopedLock lock(pathCreationLock);

        // Copy samples to display buffer
        int writePos = 0;
        if (size1 > 0)
        {
            for (int i = 0; i < size1; ++i)
                displayBuffer[writePos++] = audioBuffer.getSample(0, start1 + i);
        }
        if (size2 > 0)
        {
            for (int i = 0; i < size2; ++i)
                displayBuffer[writePos++] = audioBuffer.getSample(0, start2 + i);
        }

        abstractFifo.finishedRead(size1 + size2);
    }

    // Always repaint to show updates
    repaint();
}

void OscilloscopeComponent::paint(juce::Graphics& g)
{
    // Draw black background
    g.fillAll(juce::Colours::black);

    juce::ScopedLock lock(pathCreationLock);

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    auto centerY = bounds.getCentreY();
    auto heightScale = bounds.getHeight() * 0.4f;

    // Draw center line for reference
    g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
    g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f);

    // Create waveform path
    juce::Path waveformPath;
    bool pathStarted = false;

    for (size_t i = 0; i < displayBuffer.size(); ++i)
    {
        float x = bounds.getX() + (i / (float)displayBuffer.size()) * bounds.getWidth();
        float y = centerY - (displayBuffer[i] * heightScale);

        if (!pathStarted)
        {
            waveformPath.startNewSubPath(x, y);
            pathStarted = true;
        }
        else
        {
            waveformPath.lineTo(x, y);
        }
    }

    // Draw glow effect with multiple passes
    // Outer glow (most transparent, thickest)
    g.setColour(juce::Colour(255, 0, 0).withAlpha(0.1f));
    g.strokePath(waveformPath, juce::PathStrokeType(6.0f));

    // Middle glow
    g.setColour(juce::Colour(255, 0, 0).withAlpha(0.3f));
    g.strokePath(waveformPath, juce::PathStrokeType(3.0f));

    // Inner glow
    g.setColour(juce::Colour(255, 50, 50).withAlpha(0.6f));
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));

    // Core line (brightest, thinnest)
    g.setColour(juce::Colour(255, 100, 100));
    g.strokePath(waveformPath, juce::PathStrokeType(1.0f));

    // Debug: Show buffer status
    g.setColour(juce::Colours::white);
    g.setFont(10.0f);
    g.drawText("Samples: " + juce::String(abstractFifo.getNumReady()),
               bounds.getX(), bounds.getY(), 100, 15, juce::Justification::left);
}

void OscilloscopeComponent::resized()
{
    // Nothing to do here for now
}
