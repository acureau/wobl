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

This is one piece of the puzzle, but how do we actually describe this movement mathematically? We use two variables: time and frequency. Time refers to the time in seconds at which we want to plot a point (or sample). Frequency refers to the amount of complete oscillation cycles per second. One full cycle of the circle is equal to `2*PI` radians, and we multiply this by `frequency * time` (the amount of cycles that have occured at the given time) to get the input angle for the sine function. We can specify the "starting point" along the circle by providing an offset called phase. Finally, since the sine function returns a value between -1 and 1 we multiply it by another variable called amplitude, which controls the amplitude range of the resulting sine wave. We perceive this as volume. Putting it all together we get the following formula.

`A \* sin(2 \* pi \* f \* t + p)`
