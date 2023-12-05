/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VariSlopeAudioProcessorEditor::VariSlopeAudioProcessorEditor (VariSlopeAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    startTimer(100);
    
    setSize (plugin_width, plugin_heigth);
    
    gainSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow,true, 100, 25);
    gainSlider.setRange(-48.0, 0.0, 0.1);
    gainSlider.setValue(- 1.0);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    
    cutOffSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    cutOffSlider.setTextBoxStyle(Slider::TextBoxBelow,true,100,25);
    cutOffSlider.setRange(20.0f, 20000.0f, 1.0f);
    cutOffSlider.setSkewFactorFromMidPoint(5000.0f);
    //cutOffSlider.setValue(20000.0);
    cutOffSlider.addListener(this);
    addAndMakeVisible(cutOffSlider);
    
    cutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,CUTOFF_ID, cutOffSlider);
    
    spreadSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    spreadSlider.setTextBoxStyle(Slider::TextBoxBelow,true,100,25);
    spreadSlider.setRange(1.001f, 20.0f, 0.1f); //TODO consider range?
    spreadSlider.setValue(2.0f);
    //spreadSlider.setSkewFactorFromMidPoint(50.0);
    spreadSlider.addListener(this);
    addAndMakeVisible(spreadSlider);
    
    spreadSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,SPREAD_ID, spreadSlider);

}

VariSlopeAudioProcessorEditor::~VariSlopeAudioProcessorEditor()
{
    
}

//==============================================================================
void VariSlopeAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setFont  (30.0f);
 
    g.drawFittedText ("GentlyWeeps", 10, 0, getWidth(), 30, juce::Justification::topLeft, 1);
    
    g.setFont (20.0f);
    
    g.drawFittedText ("Continously variable slope filter", 10, 50, getWidth(), 30, juce::Justification::topLeft, 1);
    
    g.setFont (20.0f);
    g.setColour(Colour(255,0,0));
    
    g.drawFittedText (limit_active_text, -20, 0, getWidth(), 30, juce::Justification::topRight, 1);
    
}

void VariSlopeAudioProcessorEditor::resized()
{   
    // set bound has arguments x,y,width,heigth
    cutOffSlider.setBounds(plugin_width*0.05, plugin_heigth*0.1, plugin_width*0.25, plugin_heigth*0.8);
    
    spreadSlider.setBounds(plugin_width*0.35, plugin_heigth*0.1, plugin_width*0.25, plugin_heigth*0.8);
    
    gainSlider.setBounds(plugin_width*0.65, plugin_heigth*0.1, plugin_width*0.25, plugin_heigth*0.8);
}

void VariSlopeAudioProcessorEditor::timerCallback()
{
    if ( processor.limit_active.get())
    {
        limit_active_text = "LIMITER\n ACTIVE";
        repaint();
        processor.limit_active.set(false);
        limiter_warning_timer += 1;
    }
    else if (limiter_warning_timer > 0 and limiter_warning_timer<30)
    {
        limiter_warning_timer +=1;
    }
    else if (limiter_warning_timer >= 30)
    {
        limit_active_text = "";
        limiter_warning_timer = 0;
        repaint();
    }
}

////////////////////// CONTROL SECTION //////////////////////

void VariSlopeAudioProcessorEditor::sliderValueChanged (Slider *slider)
{/*
    ///
    
    if (slider == &spreadSlider)
    {
        processor.spread_factor = spreadSlider.getValue();
        // set alpha values
        for (int alpha_num = 0; alpha_num < processor.num_filter_pairs*2; ++alpha_num)
        {
            temp_alpha = processor.cut_off * pow(processor.spread_factor, alpha_num) * (1.0 / 20000.0);
            
            // hard limit alpha to lie below value corresponding to Nyquist
            if (temp_alpha > 0.96)
            {
                temp_alpha = 0.96 + alpha_num*0.01;
            }
            processor.alphas[alpha_num] = temp_alpha;
        }
    }    
    
    if (slider == &cutOffSlider)
    {
        temp_cutoff = cutOffSlider.getValue();
        // hard limit max cut off
        if (temp_cutoff > 20000)
        {
            temp_cutoff = 20000;
        }
        processor.cut_off = temp_cutoff;
        
        // set alpha values
        for (int alpha_num = 0; alpha_num < processor.num_filter_pairs*2; ++alpha_num)
        {
            temp_alpha = processor.cut_off * pow(processor.spread_factor, alpha_num) * (1.0 / 20000.0);
            if (temp_alpha > 0.9)
            {
                temp_alpha = 0.9 + alpha_num*0.1;
            }
            processor.alphas[alpha_num] = temp_alpha;
        }
    }
    if (slider == &gainSlider)
    {
        processor.gain = pow(10, gainSlider.getValue() / 20);
    }
  */
}
 
