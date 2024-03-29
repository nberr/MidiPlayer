/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class MidiPlayerAudioProcessor
:   public juce::AudioProcessor
{
public:
    //==============================================================================
    MidiPlayerAudioProcessor();
    ~MidiPlayerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void loadMIDIFile(juce::File f);
    void calculateForBPM();
    
#ifdef JUCE_DEBUG
    //==============================================================================
    juce::StringArray debugMessages;
#endif
    
    //==============================================================================
    juce::ValueTree state;
    
private:
    //==============================================================================
    juce::Array<bool> midiIn;
    juce::Array<bool> midiOut;
    
    /*
     * default bpm is 120 according to MIDI standard
     * https://majicdesigns.github.io/MD_MIDIFile/page_timing.html
     */
    double bpm = 120.0;
    
    int blockSize = -1;
    
    int startSample = 0;
    int endSample = 0;
    int samplesPassed = 0;
    
    bool fileChanged = false;
    
    //==============================================================================
    juce::CriticalSection processLock;
    
    juce::File file;
    juce::MidiFile midiFile;
    juce::MidiBuffer midiBuffer;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiPlayerAudioProcessor)
};
