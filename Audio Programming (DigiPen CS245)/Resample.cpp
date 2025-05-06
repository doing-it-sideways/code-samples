/*****************************************************************************
  A class that would take input data from the AudioData class and resample it,
  producing a unique output without modifying the AudioData itself.
  Some details redacted to prevent future students in this class from seeing
  this code and using it to cheat in the class.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include "AudioData.h"

class Resample {
public:
    explicit Resample(const AudioData *ad, unsigned channel,
                      float factor, unsigned loop_bgn, unsigned loop_end)
        ...
    {}
    
    float output() {
        double index = findex;

        if (looping started)
            index = keep between iloop_bgn and iloop_end;

        handle case where the index isnt valid

        unsigned floorIndex = index;

        float sample = audio_data->sample(floorIndex, ...);
        float sample_next = audio_data->sample(floorIndex + 1, ...);

        handle case where sample_next isnt valid

        return lerp(between sample and sample_next using the decimal value of index);
    }

        // Move findex to next sample, taking into account the speedup and multiplier.
    void next() { ... }
    
        // Would set the frequency multiplier using the value provided in units cents
    void pitchOffset(float cents) { ... }
    
    void reset() { fIndex = 0; }
private:
    const AudioData *audio_data;
    unsigned ichannel;
    double findex;
    float speedup,
          multiplier;
    unsigned iloop_bgn, iloop_end;
};
