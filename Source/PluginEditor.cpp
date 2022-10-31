/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiPlayerAudioProcessorEditor::MidiPlayerAudioProcessorEditor(MidiPlayerAudioProcessor& p)
:   AudioProcessorEditor (&p),
    audioProcessor (p),
    content(&p)
{
    /* add the content to the scene */
    /* the content will be scaled by the editor */
    addAndMakeVisible(content);
    
    /* initialize editor */
    /* editor should be resizable...*/
    setResizable(true, true);
    
    /* ...but we want it to retain a specific aspect ratio */
    juce::ComponentBoundsConstrainer *constrainer = getConstrainer();
    
    /* set the min and max sizes */
    constrainer->setSizeLimits(400, 300, 800, 600);
        
    /* set the aspect ratio */
    constrainer->setFixedAspectRatio(400.f / 300.f);
    
    /* set the default size */
    setSize (400, 300);
    content.setBounds(0, 0, getWidth(), getHeight());
}

void MidiPlayerAudioProcessorEditor::resized()
{
    /* get the scale factor and set the transform */
    float factor = static_cast<float>(getHeight()) / static_cast<float>(300);
    content.setTransform(juce::AffineTransform::scale(factor));
}
