/*
    This class needs to implement re-sampling for source / sink mismatches in sample rate, bits per sample, sample type.
    It needs to implement channel mapping for source / sink mismatches in channel count.
    It needs to implement an N-bit array of interleaved samples, populated by a single source (the synth engine) and read by multiple sources (output drivers) through an API.
    (This array will need to be just a big chunk of memory, or we could assume an 8-bit byte is the smallest sample size.)
    It needs to implement driver management, and a driver update thread loop.

    Sample bit depth stepping isn't a cast, we need to create a new value proportional to the min and max of the destination data type.
    A sample frame is a sample at the same time on every channel.
*/

#include "OutputFormat.hpp"

class OutputHandler
{
    // We can move these into an object, and each driver can have their own too.
    OutputFormat MasterFormat;

    // Called by the synth engine, writes samples matching the master parameters to the bufffer.
    void WriteSampleFrame();

    // Called by the consumers, performs resampling and channel mapping if necessary.
    void ReadSampleFrame();

    // We will need to re-make the buffer.
    void SetMasterFormat(OutputFormat master_format);
    
    // Constructs an output handler with the given 
    OutputHandler(OutputFormat master_format);
    ~OutputHandler();
};