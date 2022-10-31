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
static constexpr int min_width = 300;
static constexpr int min_height = 200;

static constexpr int max_width = 900;
static constexpr int max_height = 600;

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
