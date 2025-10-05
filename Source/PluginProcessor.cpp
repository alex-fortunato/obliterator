#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include "OscilloscopeComponent.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor() :
    AudioProcessor(
            BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                    .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                    .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                    ),
    parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "drive", "Drive",
            juce::NormalisableRange<float>(1.0f, 1000.0f, 0.01f, 0.3f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "asymmetry", "Asymmetry",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "suboctave", "Sub Octave",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "drywet", "Dry/Wet",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "tone", "Tone",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
            "algorithm", "Algorithm",
            juce::StringArray{"Tanh", "Foldback", "Tube"}, 0));
    return {params.begin(), params.end()};
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
        const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    currentDrive = *parameters.getRawParameterValue("drive");
    currentAsymmetry = *parameters.getRawParameterValue("asymmetry");
    currentSubOctave = *parameters.getRawParameterValue("suboctave");
    currentDryWet = *parameters.getRawParameterValue("drywet");
    currentTone = *parameters.getRawParameterValue("tone");
    currentAlgorithm = static_cast<int>(*parameters.getRawParameterValue("algorithm"));

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());


    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
        // ..do something to the data...
        // Apply asymmetric distortion to each sample:
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float drySample = inputSample; // Store original dry signal
            float processedSample;

            // At drive=1.0: pass through unaffected
            // Above drive=1.0: apply selected distortion algorithm
            if (currentDrive <= 1.0f)
            {
                processedSample = inputSample; // Unity gain, no processing
            }
            else
            {
                // Apply selected distortion algorithm
                float distortedSample;
                switch (static_cast<DistortionType>(currentAlgorithm))
                {
                    case DistortionType::Tanh:
                        distortedSample = applyTanhDistortion(inputSample, currentDrive, currentAsymmetry);
                        break;
                    case DistortionType::Foldback:
                        distortedSample = applyFoldbackDistortion(inputSample, currentDrive, currentAsymmetry);
                        break;
                    case DistortionType::Tube:
                        distortedSample = applyTubeDistortion(inputSample, currentDrive, currentAsymmetry);
                        break;
                    default:
                        distortedSample = inputSample;
                        break;
                }

                // Apply DC blocking filter to remove DC offset
                if (channel < 2)
                {
                    auto& dc = dcBlocker[channel];
                    // DC blocker: y[n] = x[n] - x[n-1] + 0.995 * y[n-1]
                    processedSample = distortedSample - dc.x1 + 0.995f * dc.y1;
                    dc.x1 = distortedSample;
                    dc.y1 = processedSample;
                }
                else
                {
                    processedSample = distortedSample;
                }
            }

            // Sub-octave generation using octave divider
            float subOctaveSample = 0.0f;
            if (currentSubOctave > 0.0f && channel < 2)
            {
                auto& state = octaveState[channel];

                // Zero-crossing detection with hysteresis
                bool currentPositive = processedSample > 0.0f;

                // Flip the flip-flop on positive-going zero crossings
                if (currentPositive && !state.lastPositive)
                {
                    state.flipFlop = !state.flipFlop;
                }
                state.lastPositive = currentPositive;

                // Generate sub-octave square wave
                float rawSubOctave = state.flipFlop ? 1.0f : -1.0f;

                // Apply simple lowpass filtering to smooth the square wave
                float cutoff = 0.1f; // Adjust for smoothness
                state.lowpassZ1 += cutoff * (rawSubOctave - state.lowpassZ1);
                // Use independent amplitude so sub-octave is always audible
                subOctaveSample = state.lowpassZ1 * 0.3f;
            }

            // Add sub-octave to processed signal
            float wetSample = processedSample + (subOctaveSample * currentSubOctave);

            // Apply tone filter (tilt EQ)
            if (channel < 2)
            {
                auto& tone = toneState[channel];

                // Tone control: 0.0 = dark, 0.5 = flat, 1.0 = bright
                // Use simple one-pole lowpass and highpass filters

                // Lowpass for dark tone
                float lpCutoff = 0.3f;
                tone.lowpassZ1 += lpCutoff * (wetSample - tone.lowpassZ1);

                // Highpass for bright tone (using difference equation)
                float hpCutoff = 0.05f;
                float highpassOut = wetSample - tone.highpassX1 + 0.95f * tone.highpassZ1;
                tone.highpassZ1 = highpassOut;
                tone.highpassX1 = wetSample;

                // Mix between lowpass (dark) and highpass (bright) based on tone knob
                if (currentTone < 0.5f)
                {
                    // Blend from full lowpass (0.0) to flat (0.5)
                    float blend = currentTone * 2.0f; // 0.0 to 1.0
                    wetSample = tone.lowpassZ1 * (1.0f - blend) + wetSample * blend;
                }
                else
                {
                    // Blend from flat (0.5) to full highpass (1.0)
                    float blend = (currentTone - 0.5f) * 2.0f; // 0.0 to 1.0
                    wetSample = wetSample * (1.0f - blend) + highpassOut * blend;
                }
            }

            // Apply dry/wet mixing
            // currentDryWet = 0.0 (left): 100% dry
            // currentDryWet = 1.0 (right): 100% wet
            channelData[sample] = drySample * (1.0f - currentDryWet) + wetSample * currentDryWet;
        }
    }

    // Send output to oscilloscope (use left channel for mono display)
    if (oscilloscopeComponent != nullptr && buffer.getNumChannels() > 0)
    {
        oscilloscopeComponent->pushBuffer(buffer.getReadPointer(0), buffer.getNumSamples());
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(
            getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void AudioPluginAudioProcessor::setOscilloscopeComponent(OscilloscopeComponent* osc)
{
    oscilloscopeComponent = osc;
}

//==============================================================================
// Distortion algorithm implementations
float AudioPluginAudioProcessor::applyTanhDistortion(float input, float drive, float asymmetry)
{
    // Apply asymmetric bias before distortion
    float biasedInput = input + asymmetry * 0.5f;
    return std::tanh(drive * biasedInput);
}

float AudioPluginAudioProcessor::applyFoldbackDistortion(float input, float drive, float asymmetry)
{
    // Wave folding algorithm
    // Scale input by drive amount (use moderate scaling)
    float scaledInput = input * std::sqrt(drive);

    // Asymmetric folding: adjust thresholds based on asymmetry parameter
    // Positive asymmetry = higher positive threshold, lower negative threshold
    // Negative asymmetry = lower positive threshold, higher negative threshold
    float positiveThreshold = 1.0f + asymmetry * 0.5f;
    float negativeThreshold = 1.0f - asymmetry * 0.5f;

    // Apply asymmetric wave folding with reflection
    float foldedSample = scaledInput;
    int maxIterations = 20; // Prevent infinite loops
    for (int i = 0; i < maxIterations; ++i)
    {
        if (foldedSample > positiveThreshold)
            foldedSample = 2.0f * positiveThreshold - foldedSample;
        else if (foldedSample < -negativeThreshold)
            foldedSample = -2.0f * negativeThreshold - foldedSample;
        else
            break; // No more folding needed
    }

    // Simple output scaling to maintain reasonable levels
    return foldedSample * 0.8f;
}

float AudioPluginAudioProcessor::applyTubeDistortion(float input, float drive, float asymmetry)
{
    // Apply asymmetric bias before distortion
    float biasedInput = input + asymmetry * 0.5f;

    // Scale input by drive amount with high sensitivity for extreme saturation
    // Use sqrt to match the intensity curve, with aggressive multiplier
    float scaledInput = biasedInput * std::sqrt(drive) * 5.0f;

    // Tube distortion using exponential saturation
    // Positive and negative sides have different characteristics (asymmetric)
    float output;
    if (scaledInput >= 0.0f)
    {
        // Positive side: softer compression
        output = 1.0f - std::exp(-scaledInput);
    }
    else
    {
        // Negative side: slightly harder compression (tube characteristic)
        output = -1.0f + std::exp(scaledInput * 1.2f);
    }

    // Apply gentle compression to tame peaks
    output = output * 0.85f;

    return output;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
