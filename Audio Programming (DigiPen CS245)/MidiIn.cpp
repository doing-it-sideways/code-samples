/*****************************************************************************
  An interface that would handle the event loop for the PortMidi library.
  Some details redacted to prevent future students in this class from seeing
  this code and using it to cheat in the class.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <string>
#include <thread>
#include <portmidi.h>

// This is an interface.
class MidiIn {
public:
    MidiIn(int ...)
    : ...
    , process_events(false)
    , thread_running(true)
    {
        initialize portmidi library
        initialize portmidi input_stream
        initialize event thread
    }

    void start() { ... }
    void stop() { ... }

    // Close threads, handle library cleanup
    virtual ~MidiIn(void) { ... }

    // Functions meant to be overwritten. Although I would probably mark these as = 0,
    // for good reason at the time, the professor had provided an empty implementation.
    // This allowed for us to test that we could call the correct functions properly
    // when implementing the handling of events.
    virtual void onNoteOn(int channel, int note, int velocity) { }
    virtual void onNoteOff(int channel, int note) { }
    virtual void onPitchWheelChange(int channel, float value) { }
    virtual void onVolumeChange(int channel, int level) { }
    virtual void onModulationWheelChange(int channel, int value) { }
    virtual void onControlChange(int channel, int number, int value) { }
    virtual void onPatchChange(int channel, int value) { }

private:
    bool process_events,
        thread_running;

    static void eventLoop(MidiIn *mptr) {
        //instead of needing to do bit manipulation, just store the event in the data already separated
        union Message {
            int32_t message;
            uint8_t bytes[4];
        };

        while (mptr->thread_running) {
            if (!mptr->process_events)
                continue;

            PmEvent event;
            int eventsRead = Pm_Read(..., 1);

            handle errors from read
            
            if (eventsRead == 0)
                continue;

            // Separate the bytes in the event
            Message msg;
            msg.message = event.message;

            int msgNum = ...;
            int channel = ...;

            switch (msgNum) {
                this is just handling certain events from the midi application
                whether to turn the output of a note off, on, channge controls,
                change the pitch wheel, etc.
                each case would simply call the virtual function.
                ex: case 0x00: mptr->onNothingChange(channel, ...);
                heres some more notable ones:

            case 0xB:
            { // new scope
                int control = ...;
                int val = ...;

                switch (control) {
                case 0x07: // volume change
                    ...
                    break;
                case 0x01: // modulation change
                    ...
                    break;
                default: // some other control change
                    ...
                    break;
                }
            }
            break;

            case 0xE: // pitch wheel change
            { // new scope
                constexpr int midpoint = ...;
                constexpr float floatConversion = ...;

                // retrieve 14 bit value and convert to float
                int val = ...
                float floatVal = (val - midpoint) * floatConversion;

                mptr->onPitchWheelChange(channel, floatVal);
            }
            break;

            }

        }
    }

};
