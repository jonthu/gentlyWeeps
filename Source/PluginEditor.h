/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

class VariSlopeAudioProcessorEditor : public AudioProcessorEditor,
juce::Timer,
juce::Slider::Listener
{
public:
    VariSlopeAudioProcessorEditor (VariSlopeAudioProcessor&);
    ~VariSlopeAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider *slider) override;
    
    void timerCallback() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    juce::Atomic<bool> limitStatusChanged { false };
    
    
    juce::Slider gainSlider;
    juce::Slider cutOffSlider;
    juce::Slider spreadSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadSliderAttachment;
    
    int plugin_width = 400;
    int plugin_heigth = 300;
    
    int limiter_warning_timer = 0;
    
    std::string limit_active_text = "";
    
    VariSlopeAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariSlopeAudioProcessorEditor)
    
    
};
