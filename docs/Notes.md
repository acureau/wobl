### What is sound?

Imagine clapping your hands in a full bathtub. Water is pulled towards your hands. Waves sway back and forth, increasingly subtly, until the water returns to stillness. We can see and feel the displacement in the water. Imagine now that you clap your hands in the air. The displacement that occurs isn't so obvious, we don't see the air sway back and forth. We don't feel it pulling our hands along. However, you do _hear_ a booming sound. Our ears are sensors, fine-tuned by evolution, for detecting changes in air pressure over time.

- This back and forth change is called oscillation.

### How does a speaker make sound?

A speaker simply converts an electric signal into physical movement, causing changes in air pressure that we sense as sound. The [mechanical aspects](https://www.soundguys.com/how-speakers-work-29860/) of this are interesting to read about, but for our purposes mostly irrelevant. All we need to know is that the part of the speaker displacing air, the diaphragm, is pushed and pulled between a fixed range. The electric signal controlling it also moves between a fixed low and high point, which maps directly to the displacement of the diaphragm (its offset from equilibrium).

- Make it more clear that the electric signal controls the movement of the diaphragm.

### How is sound represented digitally?

When a speaker makes sound the output is analog (continuous), but a digital computer cannot represent a purely analog signal. This is because an analog signal has infinite resolution, and a digital computer has finite space. To represent audio digitally we _must_ lose resolution. This means we have to sample the amplitude of the sound at a reasonable rate (the sample rate) to maintain accuracy. A commonly chosen sampling rate is 44,100 samples per second, around double the rate required to capture all sound audible to the human ear. A digital computer will feed a collection of amplitude samples through a DAC (digital to analog converter), then to speakers to produce sound.

- This method of encoding audio is known as PCM.

### How is digital audio synthesized?

Analog sampling makes a lot of sense when thought about in terms of recording an analog signal, such as input from a mircophone. It starts to make less sense when considering how to synthesize new audio. I wasn't entirely honest when I said a digital computer could not represent an analog signal. It cannot _store_ an analog signal with infinite resolution, but it can _generate_ that signal algorithmically. That generated signal can be sampled.

### Generating a sine wave.

A sine wave is the natural point to start synthesizing audio because it represents a pure frequency, meaning that it oscillates at a constant speed. This produces a constant tone. At the core of the sine wave is the sine function.

Draw a circle on a piece of paper and put a point at its center. This center point is at Y coordinate zero, the top of your circle is at Y coordinate 1, and the bottom of your circle is at Y coordinate -1. Draw a line from the center to the right edge of the circle. Put your finger where this line ends and move it counter-clockwise along the circle, a line between your fingertip and the center of the circle form an angle with the line you drew. This angle is the input the sine function expects, and the Y coordinate of your fingertip is what it returns.Now imagine you move your finger at a constant rate along the circle's circumference and plot it out on a graph. The Y axis representing the height of your finger at each second, and the X axis representing time. The resulting graph would plot a sine wave.

This is one piece of the puzzle, but how do we actually describe this movement mathematically? We use two variables: time and frequency. Time refers to the time in seconds at which we want to plot a point (or sample). Frequency refers to the amount of complete oscillation cycles per second. One full cycle of the circle is equal to `2*PI` radians, and we multiply this by `ft` (the amount of cycles that have occured at the given time) to get the input angle for the sine function. We can specify the "starting point" along the circle by providing an offset called phase. Finally, since the sine function returns a value between -1 and 1 we multiply it by another variable called amplitude, which controls the amplitude range of the resulting sine wave. We perceive this as volume. Putting it all together we get the following formula.

`A * sin(2πft + p)`

### Generating More Primitives

So, I've skipped a lot of notes since last time. I've implemented three more waveforms: square, sawtooth, and triangle. I've also added a noise function.

The second oscillator I implemented was the square wave. Each cycle the square wave alternates between the peak and trough. I determine the current cycle with `ft`, and multiply it by two to represent both "states" of a cycle. `floor(2ft % 2)` gives either a zero or one representing the lower and upper bounds of the waveform. To support negative frequency and time values we use the absolute value. The final formula is `|floor(2ft % 2)|`.

The third oscillator I implemented was the sawtooth wave. Each cycle the sawtooth wave starts at the trough and climbs to the peak. Again, I determine the current cycle with `ft`. `ft % 1` a number between 0 and 1 corresponding to how far along the current cycle we are, and that's our amplitude. Again, we use the absolute value to support negative time and frequency, and the final formula is `|ft % 1|`.

The final oscillator I implemented was the triangle wave. Each cycle the triangle wave climbs from its trough to its peak, and then back down to its trough. Again, I determine the current cycle with `ft`. I multiply it by two to represent both states, and take the result modulo two to determine the current state. `2ft % 2` gives a value `v` between 0 and 2. `|v - 1|` reflects the value around 1, so that 0 and 2 map to 1, and 1 maps to 0. We subtract this reflected value from 1 to invert the map, and so the final formula is `1 - |2ft % 2 - 1|`.

Noise is extremely simple, we're just generating a random amplitude between the -1 and 1 bounds.

### Standardizing Oscillators

I'm writing this after the fact, but I realized while coming up with my oscillator functions that I was sort of just inserting phase offset wherever. It was measured in radians in my sine function, and cycles in my square function. I wanted the same offset to correlate to the same shift when passed through any of my oscillators, so I decided to standardize on measuring phase offset in cycles. I also adjusted the return value of my other oscillators to match the -1 and 1 output by the sine function.

### Phase Modulation

The first thing I wanted to do was modulate the frequency of one waveform with another. I expected this to create a vibrato type effect. I created a low frequency sine wave and used it to modulate the frequency of another sine wave. However, the output was a terrible wobbling noise which seemed to climb in pitch indefinitely. I opened up the waveform in audacity and I noticed the issue. The phase at which we're changing frequency is changing over time... this makes it drift. We need to shift frequency while keeping the waveform symmetric, and as it turns out there's a trick often used to do so.

If we instead modulate the phase offset we get symmetric frequency modulation. At first this doesn't make any sense, but consider what's actually happening as we offset phase in time. As phase offset decreases our waveform is moved back in time. Samples that we'd get in the future we're getting now. As it increases our waveform is moved forward in time, and we're getting samples from the past. Of course when we think about a waveform we're thinking about samples plotted over time, but that doesn't _really_ exist. Offsetting phase isn't going to chaotically throw a bunch of samples back and forth. It's going to change the amplitude at the current phase.

### From Theory to Code

I made a severe mistake in assuming that if my oscillator functions were mathematically sound, they'd be very easy to translate to code. As I have found out that was incorrect. There are lots of issues using floating point values. I need to do a manual check for absolute zero frequencies, I need to use special versions of functions with weird behavior. For example, `fmodf` retains the sign of the first argument, meaning I need to adjust negative return values.

About a week of work later I've encountered a frequency drift that builds as time increases, and I'm beginning to suspect this is a floating point precision error. After some research I have stumbled upon "phase accumulation", where a static phase increment is calculated once and applied consistently. This has the unfortunate downside of limiting frequency modulation, since it's a variable in the incrementer equation you need to recalculate the phase increment. It also has another unfortunate downside: requiring the complete redesign of my oscillator API!

I bit the bullet and transitioned to C++, it is a few days later and I have finished re-working the oscillators. I had to make some trade-offs to implement the phase increment approach. The biggest one being that oscillators
now need to be aware of the rate at which they're being sampled. Kind of annoying but not the end of the world. I also cannot update the frequency or sample rate of an oscillator without recalculating the phase increment. I was kind of annoyed that I couldn't mess with time anymore, since the oscillator functions are no longer "pure". I added a "speed" multiplier applied to the pre-calculated phase increment every time phase is incremented.

So really I've lost some flexibility and I haven't gone anywhere, but the high-frequency "pollution" that appeared as time increased no longer appears. My concerns about the speed switching over to C++ were not warranted. Perhaps it's slower than C in some ways, but I can generate 10 minutes of audio, a sine wave with phase offset and speed modulation, in under one second. At this point we're definitely within the real-time window.

### Frequency Content

What does "frequency content" mean? Don't our synthesized waveforms have only a single frequency? To say that the answer is complicated would be an understatement. A sine wave is "pure", it is a perfect representation of oscillation at a specific frequency. More complex waveforms, such as a square wave, can be decomposed into constituent sine waves using more complex math than we're going to need at the moment. Just understand that frequency content refers to these frequencies in complex waveforms.

### Low-Pass Filtering

Low-pass filters allow low frequencies to "pass", filtering frequencies on the high end. Your first thought may be that in order to filter high frequencies we need to first decompose the waveform, and while this is definitely _one_ way to do so, it is not the only way. A low-pass filter you (programming reader) may be more familiar with is the rolling average. For those unfamiliar a rolling average is a simple way to smooth sequential inputs by storing a fixed-length window of inputs, where the newest input is inserted and the oldest input removed (rolling), then the entire collection averaged to produce a "smoothed" value.

Learning this blew my mind. How is it possible that this removes high frequencies from a waveform? Well, remember that frequency represents the amount of complete oscillations per second. Low frequencies move slow, and high frequencies move fast. By definition there will be more samples of low frequencies in a complex waveform than high frequencies. The transition from low to high will be abrupt, and the transition from high to low will happen quickly. Exactly the kind of thing a rolling average is used to "smooth out". "Smoothing out" is the _same thing_ as "filtering high frequencies".

Unfortunately this is computationally inefficient. You've got to store a lot of samples, each of those samples has to factor into your filtering calculation. What if there was some way to account for a bunch of past samples without storing all of them? Luckily there is! We just need to store one sample, the previous sample, and we can use it to process the current sample. All we have to do is take some percentage (0.0 to 1.0) of the current input and add it to the opposite percentage of the previous input. The result becomes the previous input, storing the "weight" of the previous inputs by proxy. This is one of the most basic low-pass filters. The final formula is as follows.

`scale * current + (1 - scale) * previous`

### High-Pass Filtering

It follows that high-pass filters allow high frequencies to "pass", but the way in which we do that cannot be at all related to our "smoothing" approach to low-pass filtering, right? For a while I thought about algorithms for achieving the opposite effect. I am sure there is a way to do so, but I never made it there because the solution is surprisingly simple. We want to subtract the low frequencies from our waveform, leaving only the high frequencies. We have already extracted the low frequencies from the waveform, so we can _literally_ subtract it from the input sample.

`current - (scale * current + (1 - scale) * previous)`

One gotcha moment here, when we're using this method of high-pass filtering we don't store the high-pass result in the previous variable since we're dependant on the low-pass calculation. It's more of a little modification applied to a low-pass filter.

### Envelopes

With filtering fully implemented the last step to making my synthesizer a usable, albeit simple, musical instrument is to implement envelopes. Envelopes describe how the parameters of oscillators change over time. The most common envelope is the ADSR envelope, which normally controls amplitude. Attack (A) controls how long it takes to rise to peak amplitude, decay (D) controls how long it takes to descend to a specified sustain (S) level. Release (R) controls how long it takes to descend to zero.

An envelope will be similar to an oscillator, it will take a point in time and return a multiplier value to be applied to whatever parameter it's controlling. I've learned my lesson and I'm going to be implementing a pre-calculated time increment. It will need to be re-triggered if we want to apply it again, for example an oscillator will continue to produce samples but they will be at amplitude zero after release.

### ADSR
