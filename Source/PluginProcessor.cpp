/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VariSlopeAudioProcessor::VariSlopeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts (*this, nullptr, "Parameters", createParameters())
#endif
{
}

VariSlopeAudioProcessor::~VariSlopeAudioProcessor()
{
}



//==============================================================================
const String VariSlopeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VariSlopeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VariSlopeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VariSlopeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VariSlopeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VariSlopeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VariSlopeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VariSlopeAudioProcessor::setCurrentProgram (int index)
{
}

const String VariSlopeAudioProcessor::getProgramName (int index)
{
    return {};
}

void VariSlopeAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VariSlopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto s = apvts.getRawParameterValue(SPREAD_ID);
    spread_factor = s->load();
    prev_spread_factor = spread_factor;
    
    auto c = apvts.getRawParameterValue(CUTOFF_ID);
    temp_cutoff = c->load();
    prev_cutoff = temp_cutoff;
    
    for (int alpha_num = 0; alpha_num < num_filter_pairs*2; ++alpha_num)
    {
        alphas[alpha_num] = cut_off * pow(spread_factor, alpha_num) * (1.0 / 20000.0);
    }
    
    limit_active.set(false);
    
}

void VariSlopeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VariSlopeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VariSlopeAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage)
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // iterate over channels
    for (int channel = 0; channel < buffer.getNumChannels() ; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        channel_shift = num_filter_pairs * channel;
       
        // iterate over samples
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // 10 hz low filter of parameter changes ***
            auto s = apvts.getRawParameterValue(SPREAD_ID);
            spread_factor = s->load();
            
            
            if (spread_factor != prev_spread_factor)
            {
                spread_factor = p_alpha*spread_factor + (1 - p_alpha) * prev_spread_factor;
                prev_spread_factor = spread_factor;
            }
            
            auto c = apvts.getRawParameterValue(CUTOFF_ID);
            temp_cutoff = c->load();
            
            if (temp_cutoff != prev_cutoff)
            {
                temp_cutoff = p_alpha*temp_cutoff + (1 - p_alpha) * prev_cutoff;
                prev_cutoff = temp_cutoff;
            }

            // hard limit max cut off
            if (temp_cutoff > 20000)
            {
                temp_cutoff = 20000;
            }
            cut_off = temp_cutoff;
            
            // set alpha values
            for (int alpha_num = 0; alpha_num < num_filter_pairs*2; ++alpha_num)
            {
                temp_alpha = cut_off * pow(spread_factor, alpha_num) * (1.0 / 20000.0);
                if (temp_alpha > 0.9)
                {
                    temp_alpha = 0.9 + alpha_num*0.1;
                }
                alphas[alpha_num] = temp_alpha;
            }
            for (int filter_pair = 0; filter_pair < num_filter_pairs; ++filter_pair)
            {
                ////////////////////// LOW PASS SECTION //////////////////////

                // fetch either plugin input or output from end of previous filter pair
                if (filter_pair == 0)
                {
                    input = buffer.getSample(channel, sample);
                }
                else
                {
                    input = output;
                }
                
                alpha = alphas[filter_pair];
                
                // filter signal
                output = alpha*input + (1 - alpha) * prev_lp_outputs[filter_pair + channel_shift];
                
                prev_lp_outputs[filter_pair + channel_shift] = output;
                
                ////////////////////// INVERSE LOW PASS SECTION //////////////////////

                input = output;

                alpha = alphas[filter_pair + 1];
                
                // filter signal
                output = ( input + (alpha - 1) * prev_inv_inputs[filter_pair + channel_shift] ) / alpha;

                prev_inv_inputs[filter_pair + channel_shift] = input;
                
                
            }
            
            // hard limit audio out
            if (abs(output) > 2.0)
            {
                output = copysign(2.0, output);
                limit_active.set(true);
            }


            channelData[sample] = output;
        }
        
    }

}

//==============================================================================
bool VariSlopeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VariSlopeAudioProcessor::createEditor()
{
    return new VariSlopeAudioProcessorEditor (*this);
}

//==============================================================================
void VariSlopeAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VariSlopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout VariSlopeAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(CUTOFF_ID, CUTOFF_NAME, 20.0f, 20000.0f, 5000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(SPREAD_ID, SPREAD_NAME, 1.001f, 20.0f, 10.0f));
    
    return { params.begin(), params.end()};
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VariSlopeAudioProcessor();
}


