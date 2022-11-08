/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiPlayerAudioProcessor::MidiPlayerAudioProcessor()
{
}

MidiPlayerAudioProcessor::~MidiPlayerAudioProcessor()
{
}

//==============================================================================
const juce::String MidiPlayerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiPlayerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiPlayerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiPlayerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiPlayerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiPlayerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MidiPlayerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiPlayerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String MidiPlayerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MidiPlayerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void MidiPlayerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    
    for (auto& note : noteHistory) {
        note = false;
    }
}

void MidiPlayerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void MidiPlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    /* not interested in audio data */
    juce::ignoreUnused(buffer);
    
    for (auto metadata : midiMessages) {
        
        /* get the message and note number */
        auto msg = metadata.getMessage();
        int note = msg.getNoteNumber();

        
        /* only interested in on and off messages */
        if (msg.isNoteOn()) {
            
            /* start sequence only if this is the first note on */
            if (!noteHistory.contains(true)) {
                /* sequence isn't currently playing so we need to start it */
            }
            
            /* turn the note on */
            noteHistory.getReference(note) = true;
        }
        else if (msg.isNoteOff()) {
           
            /* turn the note off */
            noteHistory.getReference(note) = false;
            
            /* end sequence only if all notes are off */
            if (!noteHistory.contains(true)) {
                
                /* all notes are off. stop the sequence */
                
            }
        }
    }
}

//==============================================================================
bool MidiPlayerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiPlayerAudioProcessor::createEditor()
{
    return new MidiPlayerAudioProcessorEditor (*this);
}

//==============================================================================
void MidiPlayerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void MidiPlayerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
void MidiPlayerAudioProcessor::loadMIDIFile(juce::File f)
{
    const juce::ScopedLock myScopedLock(processLock);
    
    fileBuffer.clear();
    loadedFile.clear();
    
    juce::FileInputStream s(f);
    loadedFile.readFrom(s);

    auto seq = *loadedFile.getTrack(0);
    
    for (auto metadata : seq) {
        auto msg = metadata->message;
        
        if (msg.isNoteOn()) {
            fileBuffer.addEvent(msg, static_cast<int>(msg.getTimeStamp()));
        }
        else if (msg.isNoteOff()) {
            fileBuffer.addEvent(msg, static_cast<int>(msg.getTimeStamp()));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiPlayerAudioProcessor();
}
