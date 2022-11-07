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
    // upper rectangle
    g.setColour(juce::Colour(13, 13, 13));
    g.fillRect(0, 0, 300, 60);
    
    // lower rectangle
    g.setColour(juce::Colour(26, 26, 26));
    g.fillRect(0, 60, 300, 180);
}

void EditorContent::resized()
{
   
}
