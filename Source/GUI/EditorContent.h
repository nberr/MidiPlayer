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
    //void paintOverChildren(juce::Graphics& g) override;
    void resized() override;
    
private:
    //==============================================================================
    MidiPlayerAudioProcessor* processor;
    
    //==============================================================================
    // Main
    std::unique_ptr<juce::Drawable> backplate;
    
    void showOverlay();
    juce::DrawableButton infoButton;
    
    //==============================================================================
    juce::DrawableButton dirButton;
    juce::DrawableButton prevButton;
    juce::DrawableButton nextButton;
    
    //==============================================================================
    void loadFiles();
    void loadNextFile();
    void loadPrevFile();
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Array<juce::File> midiFiles;
    int fileIndex = -1;
    
    juce::Label fileLabel;
    
    //==============================================================================
    // Overlay
    std::unique_ptr<juce::Drawable> overlayBackplate;
    
    void hideOverlay();
    juce::DrawableButton closeButton;
    juce::DrawableButton linkButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorContent)
};
