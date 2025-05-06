/*****************************************************************************
  An interface that would handle the event loop for the PortMidi library.
  Some details redacted to prevent future students in this class from seeing
  this code and using it to cheat in the class.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <array>
#include <mutex> //<-- each function in WavetableSynth locks a mutex, not shown

#include "MidiIn.h"
#include "AudioData.h"
#include "Resample.h"

// Attack, Decay, Sustain, Release curve handling class.
class ADSR {
    // a == time from silence -> max volume for the audio
    // d == time to decay from max volume to sustain level
    // s == level the sound's volume stays at while on
    // d == time it takes for a sound to silence after being turned off
    ADSR(float a = 0, float d = 0, float s = 1, float r = 0, float R = 44100) { ... }
    void sustainOff() { ... }
    void reset() { ... }
    void next() { ... }
    void output() { ... }

    ...
};

  //A list of most of the constants used in the synthesizer since another interface or
  //user input isn't part of the project.
namespace {
    constexpr const char* file = ...;
    constexpr float manualSpeedup440 = ...;
    
    // sample indices
    constexpr int fileLoopBegin = ..., fileLoopEnd = ...;
  
    constexpr double vibratoRate = ...;
    
    constexpr double twopi = 6.28318530718;
}

class WavetableSynth : private MidiIn {
public:
    WavetableSynth(int devno, int R) { ... }
    ~WavetableSynth() { ... }

    void next() {
        for each active note:
            call next() on respective members
        
        increment sampleTime
        handle floating point precision errors
    }

    void output() {
        UpdateMultipliers();

        double finalOutput = 0;

        for each active note:
            finalOutput += (some multiplier) * (note info -- sample values, any adsr curve, etc);
        
        return volume * finalOutput;
    }

private:
    // Held data that every note needs to have like amplitude, etc.
    struct Note {
        ...
        ADSR adsr { ... };
        ...
    };

       //MidiIn implementations
    void onNoteOn(int channel, int noteIndex, int velocity) override {
        float freqOffset = ...;

        Note newNote { ... };

        lock mutex here instead of at top of function

        if (activatedNotes != maxNotes) {
            set an open/finished note to this note
            update activation order
        }
        else {
            forcibly replace the oldest note with this new note
            update all other notes activation orders
        }
    }

    void onNoteOff(int channel, int noteIndex) override {
        make sure note is in the "notes" list.

        if it is in the list:
            note->adsr.sustainOff();
            --activatedNotes;
        
        update all notes activation orders
    }

    void onPitchWheelChange(int channel, float value) override { adjust pitchWheelOffset }
    void onVolumeChange(int channel, int level) override { adjust volume }
    void onModulationWheelChange(int channel, int value) override { adjust vibratoDepth }

        //updates pitch offset and vibrato offset
    void UpdateMultipliers() {
        float vibratoOffset = ...;

        update each notes pitch offset with the pitchWheelOffset and vibratoOffset
    }

    std::array<Note, maxNotes> notes{};
    std::mutex noteMutex{}; <-- each function locks the mutex, not shown
  
    AudioData ad;
  
    const int samplingRate;
    const float baseSpeedup;
  
    int sampleTime = 0;
  
    double volume = 1; //global volume modifier for all notes
    float pitchWheelOffset = 0;
    float vibratoDepth = 0;
  
    int loopBegin, loopEnd;
  
      //helps track note activation order, used for stealing notes properly
    int activatedNotes = 0;
};
