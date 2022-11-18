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
:   infoButton("info", juce::DrawableButton::ButtonStyle::ImageStretched),
    dirButton("directory", juce::DrawableButton::ButtonStyle::ImageStretched),
    prevButton("previous", juce::DrawableButton::ButtonStyle::ImageFitted),
    nextButton("next", juce::DrawableButton::ButtonStyle::ImageFitted),
    closeButton("close", juce::DrawableButton::ButtonStyle::ImageStretched),
    linkButton("link", juce::DrawableButton::ButtonStyle::ImageStretched)
{
    processor = inProcessor;
    
    // backplate
    backplate = juce::Drawable::createFromImageData(BinaryData::backplate_svg, BinaryData::backplate_svgSize);
    addAndMakeVisible(backplate.get());
    backplate->toBack();
    backplate->setBufferedToImage(true);
    
    // hamburger
    auto info_btn_data = juce::XmlDocument::parse(BinaryData::hamburger_svg);
    auto info_btn_path = juce::Drawable::createFromSVG(*info_btn_data);
    
    infoButton.setImages(info_btn_path.get());
    
    infoButton.onClick = [this] { showOverlay(); };
    
    addAndMakeVisible(infoButton);
    
    // directory icon
    auto dir_btn_data = juce::XmlDocument::parse(BinaryData::folder_svg);
    auto dir_btn_path = juce::Drawable::createFromSVG(*dir_btn_data);
    
    dirButton.setImages(dir_btn_path.get());
    
    dirButton.onClick = [this] { loadFiles(); };
    
    addAndMakeVisible(dirButton);
    
    // previous arrow
    auto prev_btn_data = juce::XmlDocument::parse(BinaryData::arrow_svg);
    auto prev_btn_path = juce::Drawable::createFromSVG(*prev_btn_data);
    
    prevButton.setImages(prev_btn_path.get());
    
    prevButton.onClick = [this] { loadPrevFile(); };
    
    addAndMakeVisible(prevButton);
    
    // next arrow
    auto next_btn_data = juce::XmlDocument::parse(BinaryData::arrow_svg);
    auto next_btn_path = juce::Drawable::createFromSVG(*next_btn_data);
    
    nextButton.setImages(next_btn_path.get());
    
    // rotation values
    // x-rot: x + (0.5f*width)
    // y-rot: y + (0.5f*height)
    nextButton.setTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi, 82.5f, 126.5f));
    
    nextButton.onClick = [this] { loadNextFile(); };
    
    addAndMakeVisible(nextButton);
    
    // file control
    fileChooser = std::make_unique<juce::FileChooser>("Please select the MIDI file you want to load...",
                                                              juce::File::getSpecialLocation(juce::File::userMusicDirectory),
                                                              "*.mid");
    
    // file display
    fileLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(240, 240, 200));
    addAndMakeVisible(fileLabel);
    
    // overlay
    overlayBackplate = juce::Drawable::createFromImageData(BinaryData::overlay_backplate_svg, BinaryData::overlay_backplate_svgSize);
    addChildComponent(overlayBackplate.get());
    
    // close
    auto close_btn_data = juce::XmlDocument::parse(BinaryData::close_svg);
    auto close_btn_path = juce::Drawable::createFromSVG(*close_btn_data);
    
    closeButton.setImages(close_btn_path.get());
    
    closeButton.onClick = [this] { hideOverlay(); };
    
    addChildComponent(closeButton);
    
    // link
    auto link_btn_data = juce::XmlDocument::parse(BinaryData::link_svg);
    auto link_btn_path = juce::Drawable::createFromSVG(*link_btn_data);
    
    linkButton.setImages(link_btn_path.get());
    
    linkButton.onClick = [] {
        if (!juce::URL(JucePlugin_ManufacturerWebsite).launchInDefaultBrowser()) {
            jassertfalse;
        }
    };
    
    addChildComponent(linkButton);
}

//==============================================================================
//void EditorContent::paintOverChildren(juce::Graphics& g)
//{
//    juce::ignoreUnused(g);
//}

void EditorContent::resized()
{
    backplate->setTransformToFit(getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
    overlayBackplate->setTransformToFit(getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
    
    infoButton.setBounds(317, 25, 13, 12);
    closeButton.setBounds(317, 26, 11, 11);
    
    dirButton.setBounds(32, 118, 18, 14);
    prevButton.setBounds(60, 119, 15, 15);
    nextButton.setBounds(75, 119, 15, 15);
    
    fileLabel.setBounds(100, 121, 160, 20);
    
    linkButton.setBounds(105, 139, 141, 11);
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

//==============================================================================
void EditorContent::showOverlay()
{
    overlayBackplate->setVisible(true);
    closeButton.setVisible(true);
    linkButton.setVisible(true);
}

void EditorContent::hideOverlay()
{
    overlayBackplate->setVisible(false);
    closeButton.setVisible(false);
    linkButton.setVisible(false);
}
