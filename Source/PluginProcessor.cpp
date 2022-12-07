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
    
    midiIn.resize(127);
    midiOut.resize(127);
    
    for (int n = 0; n < 127; ++n) {
        midiIn.getReference(n) = false;
        midiOut.getReference(n) = false;
    }
}

void MidiPlayerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void MidiPlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer);

    /* we don't really care about checking in release because we're only support AU/VST3 */
#ifdef JUCE_DEBUG
    /* if the plugin host is standalone, BPM info is missing so don't do this part */
    if (juce::PluginHostType().getPluginLoadedAs() != juce::AudioProcessor::WrapperType::wrapperType_Standalone) {
#endif
      
    /* NORMAL PROCESSING STARTS HERE */
        
    double curr_bpm = *getPlayHead()->getPosition()->getBpm();
    if (bpm != curr_bpm) {
        
        debugMessages.add("bpm changed from " + juce::String(bpm) + " to " + juce::String(curr_bpm));
        
        bpm = curr_bpm;
    }
    
    for (auto metadata : midiMessages) {
        
        /* get the message and note number */
        auto msg = metadata.getMessage();
        int note = msg.getNoteNumber();

        /* only interested in on and off messages */
        if (msg.isNoteOn()) {
            
            if (!midiIn.contains(true)) {
                bufferIndex = 0;
            }
            
            /* turn the note on */
            midiIn.getReference(note) = true;
        }
        else if (msg.isNoteOff()) {
           
            /* turn the note off */
            midiIn.getReference(note) = false;
            
        }
    }
    
    /* no notes are being held down by the user. exit early */
    if (!midiIn.contains(true)) {
        
        /* clear any notes that are still on */
        if (midiOut.contains(true)) {
            for (int n = 0; n < 127; ++n) {
                if (midiOut.getReference(n)) {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(0, n, 1.f), 0);
                }
            }
        }
        
        return;
    }
    
    midiMessages.clear();
    
    /* sequence should play */
    juce::MidiBuffer block_ref = splitFile.getReference(bufferIndex);
    midiMessages.swapWith(block_ref);
        
    for (auto metadata: midiMessages) {
        
        /* get the message and note number */
        auto msg = metadata.getMessage();
        auto note = msg.getNoteNumber();
        
        /* only interested in on and off messages */
        if (msg.isNoteOn()) {
            
            /* turn the note on */
            midiOut.getReference(note) = true;
        }
        else if (msg.isNoteOff()) {
           
            /* turn the note off */
            midiOut.getReference(note) = false;
            
        }
    }
    
    bufferIndex++;
    if (bufferIndex >= splitFile.size()) {
        bufferIndex = 0;
    }
        
#ifdef JUCE_DEBUG
    }
#endif
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
    
    /** This function call means that the MIDI file is going to be played with the
        original tempo and signature.

        To play it at the host tempo, we might need to do it manually in processBlock
        and retrieve all the time the current tempo to track tempo changes.
    */
    loadedFile.convertTimestampTicksToSeconds();

    /* for each track */
    /* NOTE: all MIDI tracks get squashed down into a single track */
    for (int t = 0; t < loadedFile.getNumTracks(); ++t) {

        auto seq = *loadedFile.getTrack(t);
        
        for (auto metadata : seq) {
            auto msg = metadata->message;
            
            if (bpm == 0) {
                bpm = 1;
            }
            
            double new_time = msg.getTimeStamp() * (60.0 / bpm);
            
            if (msg.isNoteOn()) {
                fileBuffer.addEvent(msg, static_cast<int>(new_time * getSampleRate()));
            }
            else if (msg.isNoteOff()) {
                fileBuffer.addEvent(msg, static_cast<int>(new_time * getSampleRate()));
            }
        }
    }
    
    splitFile.clear();
    
    int block = 0;
    splitFile.add(juce::MidiBuffer());
    
#if JUCE_DEBUG
    debugMessages.add("first: " + juce::String(fileBuffer.getFirstEventTime()));
#endif
    
    /* add blank blocks */
    while ((block+1) * getBlockSize() < fileBuffer.getFirstEventTime()) {
        splitFile.add(juce::MidiBuffer());
        block++;
    }
    
    
#if JUCE_DEBUG
    debugMessages.add("blank blocks: " + juce::String(block));
#endif
    
    for (auto metadata : fileBuffer) {
        
        auto msg = metadata.getMessage();
        
        if (msg.isNoteOnOrOff()) {
            
            auto time_in_block = msg.getTimeStamp() - (block*getBlockSize());
            
            if (msg.getTimeStamp() < ((block+1) * getBlockSize())) {
                splitFile.getReference(block).addEvent(msg, static_cast<int>(time_in_block));
            }
            else {
                
                while (msg.getTimeStamp() >= ((block+1) * getBlockSize())) {
                    splitFile.add(juce::MidiBuffer());
                    block++;
                }
                
                time_in_block = msg.getTimeStamp() - (block*getBlockSize());
                
                splitFile.getReference(block).addEvent(msg, static_cast<int>(time_in_block));
            }
            
#if JUCE_DEBUG
            
            debugMessages.add("note: " + juce::String(msg.getNoteNumber()));
            debugMessages.add("absolute: " + juce::String(msg.getTimeStamp()));
            debugMessages.add("relative: " + juce::String(time_in_block));
            
#endif
        }
    }
    
#if JUCE_DEBUG
    
    debugMessages.add("final: " + juce::String(fileBuffer.getLastEventTime()));
    debugMessages.add("block: " + juce::String(getBlockSize()));
    debugMessages.add("sample rate: " + juce::String(getSampleRate()));
    debugMessages.add("expected blocks: " + juce::String(fileBuffer.getLastEventTime() / getBlockSize()));
    debugMessages.add("total blocks: " + juce::String(block));
    
#endif
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiPlayerAudioProcessor();
}
