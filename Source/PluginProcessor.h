/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define CUTOFF_ID "cutoff"
#define CUTOFF_NAME "Cutoff"
#define SPREAD_ID "spread"
#define SPREAD_NAME "Spread"

//==============================================================================
/**
*/
class VariSlopeAudioProcessor  : public AudioProcessor
 
{
public:
    //==============================================================================
    VariSlopeAudioProcessor();
    ~VariSlopeAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState apvts;
    
    //==============================================================================
    
    double gain;
    double cut_off = 500;
    double alphas [6] = {0,0,0,0,0,0}; //TODO: how to define dynamically? Can alphas be redefined with new size?
    int num_filter_pairs = 3;
    double spread_factor = 2.0;
    double alpha = 0.0;
    double p_alpha = 10.0 / 20000.0;
    juce::Atomic<bool> limit_active;
      
private:
    double output = 0.0;
    double prev_output = 0.0;
    double prev_lp_outputs [6] = {0,0,0,0,0,0};
    double prev_inv_inputs [6] = {0,0,0,0,0,0};
    double input = 0.0;
    double prev_input = 0.0;
    int channel_shift = 0;
    int current_filter = 0;
    
    double temp_alpha;
    double temp_cutoff;
    
    double prev_spread_factor;
    double prev_cutoff;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariSlopeAudioProcessor)
};
