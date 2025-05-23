### Todo

- Output: Audio Sink
- Input: Sequence File

- We're actually just hiding the base ID string in drivers right now, not desirable. We want to override. Apparently we'll need to make it into a getter function.
- Put a prefix before all error messages from drivers, like we're doing in the WASAPI one.
- Convert WAV to an output driver.

### Ideas

I've decided that sequencing will involve instruments, patterns, and a timeline. I wanted to integrate programming, and I think a scripting language at the input level would work perfectly. An API for arranging and manipulating patterns. Input could be heavily customized via scripting. Any kind of peripheral or "macro" I wanted could be implemented.

The audio engine would need some kind of communication protocol, allowing us to set up instruments on the C side where all the DSP stuff is going to be implemented. Then allowing us to invoke their playback externally. This way I can use a modern GUI toolkit and still have a real-time synth. The C++ engine will do all the hard work.

### Design Weirdness

- I am applying time scaling on the oscillators but not the envelopes, maybe I should consolidate the two implementations.
- I've implemented envelope trigger smoothing, which is only really useful for the amplitude parameter. Maybe this setting should be more out of the way.
- The output handler needs some way to activate and deactivate devices, it's not ideal to write to all possible playback devices. The input handler should do the same.
- Move shared typedefs into a header.