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
:   dirButton("directory", juce::DrawableButton::ButtonStyle::ImageFitted),
    prevButton("previous", juce::DrawableButton::ButtonStyle::ImageFitted),
    nextButton("next", juce::DrawableButton::ButtonStyle::ImageFitted)
    
{
    processor = inProcessor;
    
    // directory icon
    auto dir_btn_data = juce::XmlDocument::parse(BinaryData::folder_svg);
    auto dir_btn_path = juce::Drawable::createFromSVG(*dir_btn_data);
    
    dirButton.setImages(dir_btn_path.get());
    
    addAndMakeVisible(dirButton);
    
    // previous arrow
    auto prev_btn_data = juce::XmlDocument::parse(BinaryData::left_svg);
    auto prev_btn_path = juce::Drawable::createFromSVG(*prev_btn_data);
    
    prevButton.setImages(prev_btn_path.get());
    
    addAndMakeVisible(prevButton);
    
    // next arrow
    auto next_btn_data = juce::XmlDocument::parse(BinaryData::right_svg);
    auto next_btn_path = juce::Drawable::createFromSVG(*next_btn_data);
    
    nextButton.setImages(next_btn_path.get());
    
    addAndMakeVisible(nextButton);
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
    
    // controls rectangle
    g.setColour(juce::Colour(13, 13, 13));
    g.fillRoundedRectangle(15, 110, 270, 40, 4);
    
    // upper line
    g.setColour(juce::Colour(20, 20, 20));
    g.fillRect(15, 170, 270, 2);
    
    // lower line
    g.setColour(juce::Colour(28, 28, 28));
    g.fillRect(15, 172, 270, 2);
}

void EditorContent::resized()
{
    dirButton.setBounds(30, 120, 20, 20);
    prevButton.setBounds(60, 120, 20, 20);
    nextButton.setBounds(80, 120, 20, 20);
    
    fileLabel.setBounds(100, 120, 160, 20);
}
