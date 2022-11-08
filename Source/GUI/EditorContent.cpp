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
    
    // wave icon
    waveIcon = juce::Drawable::createFromImageData(BinaryData::wave_svg, BinaryData::wave_svgSize);
    addAndMakeVisible(waveIcon.get());

    
    // directory icon
    auto dir_btn_data = juce::XmlDocument::parse(BinaryData::folder_svg);
    auto dir_btn_path = juce::Drawable::createFromSVG(*dir_btn_data);
    
    dirButton.setImages(dir_btn_path.get());
    
    dirButton.onClick = [this] { loadFiles(); };
    
    addAndMakeVisible(dirButton);
    
    // previous arrow
    auto prev_btn_data = juce::XmlDocument::parse(BinaryData::left_svg);
    auto prev_btn_path = juce::Drawable::createFromSVG(*prev_btn_data);
    
    prevButton.setImages(prev_btn_path.get());
    
    prevButton.onClick = [this] { loadPrevFile(); };
    
    addAndMakeVisible(prevButton);
    
    // next arrow
    auto next_btn_data = juce::XmlDocument::parse(BinaryData::right_svg);
    auto next_btn_path = juce::Drawable::createFromSVG(*next_btn_data);
    
    nextButton.setImages(next_btn_path.get());
    
    nextButton.onClick = [this] { loadNextFile(); };
    
    addAndMakeVisible(nextButton);
    
    // file control
    fileChooser = std::make_unique<juce::FileChooser>("Please select the MIDI file you want to load...",
                                                              juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                              "*.mid");
    
    // file display
    fileLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(240, 240, 200));
    addAndMakeVisible(fileLabel);
}

//==============================================================================
void EditorContent::paint(juce::Graphics& g)
{
    // upper rectangle
    g.setColour(juce::Colour(13, 13, 13));
    g.fillRect(0, 0, 300, 60);
    
    // plugin name
    g.setColour(juce::Colours::white);
    g.setFont(g.getCurrentFont().withHeight(24));
    g.drawText("MIDI PLAYER", 40, 19, 130, 24, juce::Justification::left);
    
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
    
    // company name
    g.setColour(juce::Colour(60, 65, 70));
    g.setFont(g.getCurrentFont().withHeight(20));
    g.drawText("SPITE", 200, 185, 90, 20, juce::Justification::centred);
}

void EditorContent::resized()
{
    auto bounds = juce::Rectangle<int>(14, 20, 20, 20);
    
    waveIcon->setTransformToFit(bounds.toFloat(), juce::RectanglePlacement::stretchToFit);
    
    dirButton.setBounds(30, 120, 20, 20);
    prevButton.setBounds(60, 120, 20, 20);
    nextButton.setBounds(80, 120, 20, 20);
    
    fileLabel.setBounds(100, 120, 160, 20);
}

//==============================================================================
void EditorContent::loadFiles()
{
    jassert(fileChooser != nullptr);
    
    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    
    /* open the file browser */
    fileChooser->launchAsync(folderChooserFlags, [this] (const juce::FileChooser& chooser)
    {
        /* get the midi file chosen */
        auto midi_file = chooser.getResult();
        
        if (!midi_file.exists()) {
            return;
        }
        
        /* search the rest of the directory for midi files */
        juce::RangedDirectoryIterator it (midi_file.getParentDirectory(), false, "*.mid");
        
        /* clear the files */
        midiFiles.clear();
        fileIndex = -1;
        
        /* add all files to the array */
        int index = 0;
        for (auto f : it) {
            midiFiles.add(f.getFile());
            
            if (f.getFile().getFileName() == midi_file.getFileName()) {
                fileIndex = index;
            }
            
            ++index;
        }
        
        fileLabel.setText(midiFiles.getReference(fileIndex).getFileName(), juce::sendNotification);
        processor->loadMIDIFile(midiFiles.getReference(fileIndex));
    });
}

void EditorContent::loadNextFile()
{
    if (fileIndex == -1) {
        return;
    }
    
    fileIndex++;
    
    if (fileIndex >= midiFiles.size()) {
        fileIndex = 0;
    }
    
    fileLabel.setText(midiFiles.getReference(fileIndex).getFileName(), juce::sendNotification);
    processor->loadMIDIFile(midiFiles.getReference(fileIndex));
}

void EditorContent::loadPrevFile()
{
    if (fileIndex == -1) {
        return;
    }
    
    fileIndex--;
    
    if (fileIndex < 0) {
        fileIndex = midiFiles.size() - 1;
    }
    
    fileLabel.setText(midiFiles.getReference(fileIndex).getFileName(), juce::sendNotification);
    processor->loadMIDIFile(midiFiles.getReference(fileIndex));
}
