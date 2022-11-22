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
    constrainer->setSizeLimits(min_width, min_height, max_width, max_height);
        
    /* set the aspect ratio */
    constrainer->setFixedAspectRatio(static_cast<float>(min_width) / static_cast<float>(min_height));
    
    /* set the default size */
    setSize (min_width, min_height);
    content.setBounds(0, 0, getWidth(), getHeight());
    
#ifdef JUCE_DEBUG
    
    // log messages every one second
    startTimer(1);
    
    debugWindow.setBounds(0, 0, 300, 200);
    debugWindow.setUsingNativeTitleBar(true);
    debugWindow.setVisible(true);
    
    debugDisplay.setMultiLine (true);
    debugDisplay.setReturnKeyStartsNewLine (true);
    debugDisplay.setReadOnly (true);
    debugDisplay.setScrollbarsShown (true);
    debugDisplay.setCaretVisible (false);
    debugDisplay.setBounds(debugWindow.getLocalBounds());
    debugWindow.Component::addAndMakeVisible(debugDisplay);
    
#endif
}

void MidiPlayerAudioProcessorEditor::resized()
{
    /* get the scale factor and set the transform */
    float factor = static_cast<float>(getHeight()) / static_cast<float>(min_height);
    content.setTransform(juce::AffineTransform::scale(factor));
}

#ifdef JUCE_DEBUG
//==============================================================================
void MidiPlayerAudioProcessorEditor::timerCallback()
{
    /* make a copy of the messages */
    juce::StringArray messages = audioProcessor.debugMessages;
    
    /* clear messages */
    audioProcessor.debugMessages.clear();
    
    /* print the messages */
    for (auto msg : messages) {
        debugDisplay.moveCaretToEnd();
        debugDisplay.insertTextAtCaret (msg + juce::newLine);
    }
}
#endif
