### We Can Generate Audio, Now What?

If you're anything like me you'd probably assume the difficult part of writing a software synthesizer would be synthesizing the audio. So much domain specific knowledge, so far from my comfort zone, how couldn't it be? It's becoming clear that I was dead wrong.

Writing software is weird that way. You know what you want in your mind, you know how you can progress in that direction, but the full scope unveils itself slowly. You never really know all the work that needs to be done until it has been completed. Imagine if I told you to build a wall clock, and you took a real "fuck it" approach to planning. You cut out a circle and draw the numbers and tick marks, after all what good is a clock that can't tell the time? Arguably the most important function of the clock.

That's about where we're at, but that's okay because now we've entered my domain.

### Design Decisions

(I want to revise this section to make clear the design desicions that led to the final result)

The idea here is to create a synthesizer engine that can be programmatically controlled, something like csound or supercollider, but of my own creation. It would be awesome to make music from nothing at all. I want it to take any form of input, write to any form of output, and most importantly to bind to a higher level language. These goals seem lofty but there will be no magic. Your computer supports any kind of hardware peripheral, but requires the implementation of a driver.

Input devices will be collections of control states. Each individual control will be asigned a unique ID which can be used to retrieve its state. Some controls, such as keyboard keys, will have two states. Others, such as knobs, will have an analog state from zero to one. These types can be extended in the future, the most important for now is the binary input.

Output devices will be more finicky, particularly in terms of who sets the audio format parameters. Some output devices like WAV files or speaker hardware will rely on the configured parameters. Some will provide the parameters they are capable of playing back. We need to decide how we're going to handle different sample data types, a global sample rate is easy enough. Right now everything is implemented as 32-bit floats.

We need an abstraction to handle a "patch". What oscillators should compose the whole? What filters should be applied? What modulators should modulate which parameters? How do we trigger it? How do we change its parameters on the fly? Will a patch also bind input devices? YES.

Configuration and usage will be completely separate from the core implementation. The core will have C APIs and Lua bindings by default. You'll use these APIs to drive the engine in different ways:

- Configuring input devices.
- Configuring output devices.
- Configuring voices.
- Configuring mixer.

We're going to continue using floats as our internal representation for now, and down-scale for output device restrictions. This is less performant but less complex than implementing some type generic core code. It will be worth revisiting this at a later time.

### Receiving Input

(input system architecture)

I just want to let it be known that the [RAWINPUTDEVICELIST](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevicelist) structure defined by the Windows API represents _a single device_.

### Generating Output

(output system architecture)

I am designing an output handler where the synthesizer engine is an authoritative provider of parameters like sample rate, bits per sample, and sample type (float or integer). The core of the system will be a single-producer multi-consumer ring buffer. I will implement a resampler that sits between the buffered samples and the consumer. 

### Abstracting Synthesis

(synthesis system architecture)

### A Higher Level Interface

(lua bindings)
