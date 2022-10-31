/*
  ==============================================================================

    EditorContent.cpp
    Created: 31 Oct 2022 1:49:51pm
    Author:  Nicholas Berriochoa

  ==============================================================================
*/

#include "EditorContent.h"

//==============================================================================
EditorContent::EditorContent(MidiPlayerAudioProcessor* inProcessor)
{
    processor = inProcessor;
}

//==============================================================================
void EditorContent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::wheat);
}

void EditorContent::resized()
{
   
}
