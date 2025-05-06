### Wavetable Synthesizer -- (Technically) A Semester Long Project
- All of the projects in audio programming built upon one another, so some of the API may seem
  a bit weird to how you might do it. I wasn't allowed to modify .h files for the most part
  so a lot of design choices are not what I would personally do.
- The program would read in audio data from an uncompressed *.wav* file and a text file with the pitch
  offset and loop points. Then using the libraries *PortAudio* and *PortMidi* would process events
  and play audio on a midi player.
- C++11, should support both linux and windows.
- Most of the files redact or put psuedocode in place of real code in order to prevent
  future students from cheating while still providing details of the work I did. 