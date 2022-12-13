/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "GUI/Content/EditorContent.h"

//==============================================================================
static constexpr int min_width = 350;
static constexpr int min_height = 225;

static constexpr int max_width = 1050;
static constexpr int max_height = 675;

//==============================================================================
class MidiPlayerAudioProcessorEditor
:
#ifdef JUCE_DEBUG
    private juce::Timer,
#endif
    public juce::AudioProcessorEditor
{
public:
    MidiPlayerAudioProcessorEditor (MidiPlayerAudioProcessor&);

    //==============================================================================
    void resized() override;

private:
    //==============================================================================
    MidiPlayerAudioProcessor& audioProcessor;
    
    EditorContent content;

#ifdef JUCE_DEBUG
    //==============================================================================
    void timerCallback() override;
    
    juce::DocumentWindow debugWindow {"Debug", juce::Colours::white, 0};
    juce::TextEditor debugDisplay;
#endif
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiPlayerAudioProcessorEditor)
};
