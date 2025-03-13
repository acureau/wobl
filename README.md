### Todo

- Output: Audio Sink
- Filters: Low-pass, High-pass
- Envelopes: Attack, Decay, Sustain, Release
- Input: Keyboard, Sequence File

### Ideas

I've decided that sequencing will involve instruments, patterns, and a timeline. I wanted to integrate programming, and I think a scripting language at the input level would work perfectly. An API for arranging and manipulating patterns. Input could be heavily customized via scripting. Any kind of peripheral or "macro" I wanted could be implemented.

The audio engine would need some kind of communication protocol, allowing us to set up instruments on the C side where all the DSP stuff is going to be implemented. Then allowing us to invoke their playback externally. This way I can use a modern GUI toolkit and still have a real-time synth. The C engine will do all the hard work.
