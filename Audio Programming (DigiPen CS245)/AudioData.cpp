/*****************************************************************************
  The class that would handle the reading in and retrieval of data from a .wave file.
  Some details redacted to prevent future students in this class from seeing
  this code and using it to cheat in the class.

  Author(s): Evan O'Bryant
  Copyright © 2024 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <cmath>
#include <vector>

class AudioData {
public:
    AudioData(unsigned nframes, unsigned R, unsigned nChannels)
        : fdata(nframes * nChannels)
        , frame_count(nframes)
        , sampling_rate(R)
        , channel_count(nchannels)
    {}

    float sample(unsigned frame, unsigned channel) const/nonconst {
        return fdata[...];
    }

    helper functions: data -> (const) float*, frames -> unsigned, rate -> unsigned, channels -> unsigned

    // Initialized audio data by reading from a file.
    AudioData(const char* fname);

private:
    std::vector<float> fdata;
    unsigned frame_count, sampling_rate, channel_count;
};

// Normalized audio data to a specified decibel value.
void normalize(AudioData& ad, float dB = 0) {
    std::vector<float> offsets(channels);

    // Since some files could hold multiple channels, this loop would go through each sample,
    // and calculate the dc offset for each channel.
    // Ex: channels == 2, channel[0] += offset[i], channel[1] += offset[i], channel[0] += offset[i]...
    for (unsigned i = 0, curChannel = 0, i < samples; ++i, ++curChannel %= channels) {
        offsets[curChannel] += *(ad.data() + i);
    }

    offsets[each channel] = offsets[each channel] / ad.frames();

    for (unsigned i = 0, curChannel = 0, i < samples; ++i, ++curChannel %= channels) {
        subtract offset for each data point
        clamp value to make sure it stays in reasonable range

        calc sourceAbsoluteMaximum
    }

    const float scaleFactor = std::pow(..., db / ...) / sourceAbsoluteMaximum;

    multiply each data point by the scale factor
}

// I used this as shorthand since binary reading/writing in C++ is a pain
#define BYTE_RW(x) (char*)&x, sizeof(x)

AudioData::AudioData(const char* fname)
    : init private vars
{
    open file

    check first four characters == "RIFF"

    uint32_t fileSize = 0;
    stream.read(BYTE_RW(fileSize));

    check next four characters == "WAVE"
    
    there are multiple data chunks, however the "fmt " chunk has to be read first, as it contains
    crucials details about the data chunks. there could also be other types of chunks, however, theyre
    not read here

    bool fmtRead = false, dataRead = false;

        //assigned in "fmt " parsing, used in "data" parsing
    int bytesPerSample;

    while (stream.is_open()) {
        read 4 bytes -> header
        read 4 bytes -> chunkSize

        if (header == "fmt ") {
            char* fmtRawData = new char[chunkSize];
            stream.read(fmtRawData, chunkSize);

            	//reinterpret raw data as pointer to an unnamed struct containing "fmt " fields
			struct {
				uint16_t audioFormat;
				uint16_t channelCount;
				uint32_t samplingRate;
				uint32_t bytesPerSecond;
				uint16_t bytesPerFrame;
				uint16_t bitsPerSample;
			} *fmtStruct;

			fmtStruct = reinterpret_cast<decltype(fmtStruct)>(fmtRawData);

            init private class vars

            handle invalid header data

            fmtRead = true;

            delete[] fmtRawData;
        }
        else if (header == "data") {
            throw if fmt chunk hasnt been read.

            read in chunk piece by piece:
                for (uint32_t i in bytesRead) {
                    // 8 bit conversion
                    if (bytesPerSample == 1) {
                            // 8 bit values can only be 0-127, 8th bit is reserved
                        int8_t sampleVal = convert data to 8 bit val.
                        fdata.push_back(float(sampleVal) / (1 << 7));
                    }
                    // 16 bit conversion
                    else {
                        int16_t sampleVal = convert data to unsigned 16 bit val
                        fdata.push_back(float(sampleVal) / UINT16_MAX);
                    }
                }
        }
        else {
            seek(...); // skip chunk if not "fmt " or "data"
        }
    }

    set frame count
}

// Serializing audio data to a .wave file. Just boilerplate writing binary data to a file.
bool waveWrite(const char* fname, const AudioData& ad, unsigned bits) { ... }
