/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "GUI/EditorContent.h"

//==============================================================================
static constexpr int min_width = 350;
static constexpr int min_height = 225;

static constexpr int max_width = 1050;
static constexpr int max_height = 675;

//==============================================================================
class MidiPlayerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MidiPlayerAudioProcessorEditor (MidiPlayerAudioProcessor&);

    //==============================================================================
    void resized() override;

private:
    //==============================================================================
    MidiPlayerAudioProcessor& audioProcessor;
    
    EditorContent content;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiPlayerAudioProcessorEditor)
};
