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
    state = juce::ValueTree("State");
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
    
    blockSize = samplesPerBlock;
    
    midiBuffer.clear();
    
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
    buffer.clear();
    
    /* we don't really care about checking in release because we're only support AU/VST3 */
#ifdef JUCE_DEBUG
    /* if the plugin host is standalone, BPM info is missing so don't do this part */
    if (juce::PluginHostType().getPluginLoadedAs() != juce::AudioProcessor::WrapperType::wrapperType_Standalone) {
#endif
      
    /* NORMAL PROCESSING STARTS HERE */
        
    double curr_bpm = *getPlayHead()->getPosition()->getBpm();
    if (bpm != curr_bpm) {
        
        bpm = curr_bpm;
        
        calculateForBPM();
    }
    
    for (auto metadata : midiMessages) {
        
        /* get the message and note number */
        auto msg = metadata.getMessage();
        int note = msg.getNoteNumber();

        /* only interested in on and off messages */
        if (msg.isNoteOn()) {
            
            if (!midiIn.contains(true)) {
                
                startSample = 0;
                endSample = midiBuffer.getLastEventTime();
                
                auto data = midiBuffer;
                midiMessages.swapWith(data);
                
                samplesPassed = 0;
            }
            
            /* turn the note on */
            midiIn.getReference(note) = true;
        }
        else if (msg.isNoteOff()) {
           
            /* turn the note off */
            midiIn.getReference(note) = false;
            
        }
    }
       
        
    if (!midiIn.contains(true)) {
        
        /* no notes being held down */
        /* need to skip the rest of the midi buffers */
        
        if (samplesPassed < endSample) {
            midiMessages.clear();
            
            midiMessages.addEvent(juce::MidiMessage::allNotesOff(0), 0);
            
            samplesPassed = endSample;
        }
        
        return;
    }
    
    if (samplesPassed > endSample) {
        
        startSample = 0;
        endSample = midiBuffer.getLastEventTime();
        
        auto data = midiBuffer;
        midiMessages.swapWith(data);
        
        samplesPassed = 0;
    }
    
    for (auto metadata: midiMessages) {
        
        /* get the message and note number */
        auto msg = metadata.getMessage();
        auto note = msg.getNoteNumber();
        
        /* only interested in on and off messages */
        if (msg.isNoteOn()) {
            
            
            debugMessages.add("note " + juce::String(note) + " on");
            /* turn the note on */
            midiOut.getReference(note) = true;
            
        }
        else if (msg.isNoteOff()) {
           
            debugMessages.add("note " + juce::String(note) + " on");
            /* turn the note off */
            midiOut.getReference(note) = false;
            
        }
    }
    
    samplesPassed += buffer.getNumSamples();
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
    
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MidiPlayerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    /* This function won't be called if the plugin has never been opened */
    
    std::unique_ptr<juce::XmlElement> state_xml (getXmlFromBinary (data, sizeInBytes));
    
    if (state_xml.get() == nullptr) {
        return;
    }
    
    state = juce::ValueTree::fromXml(*state_xml);
    
    if (state.hasProperty("Path")) {
        /* try to load the directory */
        
        juce::String path = state.getProperty("Path");
        
        juce::File dir(path);
        
        if (!dir.exists()) {
            /* file doesn't exist anymore */
            return;
        }
       
        /* directory still exists. try to load it */
        
    }
}

//==============================================================================
void MidiPlayerAudioProcessor::loadMIDIFile(juce::File f)
{
    const juce::ScopedLock myScopedLock(processLock);
    
    /* save the path to the file */
    file = f;
    
    /* save the file path to the sate */
    state.setProperty("Path", file.getFullPathName(), nullptr);
    
    /* convert the file to a MIDI file */
    juce::FileInputStream s(file);
    
    midiFile.clear();
    midiFile.readFrom(s);
    
    
    /** This function call means that the MIDI file is going to be played with the
        original tempo and signature.

        To play it at the host tempo, we might need to do it manually in processBlock
        and retrieve all the time the current tempo to track tempo changes.
    */
    midiFile.convertTimestampTicksToSeconds();
    
    /* adjust time for BPM and sample rate */
    calculateForBPM();
}

void MidiPlayerAudioProcessor::calculateForBPM()
{
    midiBuffer.clear();
    
    /* for each track */
    /* NOTE: all MIDI tracks get squashed down into a single track */
    for (int t = 0; t < midiFile.getNumTracks(); ++t) {

        auto seq = *midiFile.getTrack(t);
        
        for (auto metadata : seq) {
           
            auto msg = metadata->message;
            auto note = msg.getNoteNumber();
            auto velocity = msg.getFloatVelocity();
            
            double new_time = msg.getTimeStamp() * (120.0 / bpm);
            
            if (msg.isNoteOn()) {
                debugMessages.add("adding note " + juce::String(note) + " on");
                midiBuffer.addEvent(juce::MidiMessage::noteOn(0, note, velocity), static_cast<int>(new_time * getSampleRate()));
            }
            else if (msg.isNoteOff()) {
                debugMessages.add("adding note " + juce::String(note) + " off");
                midiBuffer.addEvent(juce::MidiMessage::noteOff(0, note, velocity), static_cast<int>(new_time * getSampleRate()));
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiPlayerAudioProcessor();
}
