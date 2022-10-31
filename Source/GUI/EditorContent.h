/*
  ==============================================================================

    EditorContent.h
    Created: 31 Oct 2022 1:49:51pm
    Author:  Nicholas Berriochoa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

//==============================================================================
class EditorContent
:   public juce::Component
{
public:
    //==============================================================================
    EditorContent(MidiPlayerAudioProcessor* inProcessor);
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    //==============================================================================
    MidiPlayerAudioProcessor* processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorContent)
};
