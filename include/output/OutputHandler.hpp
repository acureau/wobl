#pragma once

#include <cstddef>
#include <vector>
#include <functional>
#include "OutputFormat.hpp"

using SampleCallback = std::function<std::vector<std::byte>(const std::string&, const std::string&)>;

class OutputHandler
{
    private:
        // The format being written to the buffer by the synthesizer engine.
        OutputFormat MasterFormat;

        /*
            A 10ms ring buffer holding sample frames produced by the synthesizer engine. 
            The producer will proceed without knowledge of consumers, and consumers will never be allowed to exceed the master frame index.
            By exceed, I mean if one more increment pushes it in front of the master index we will not proceed.
            If the producer surpasses the consumer, the data is just presumed overwritten.
        */
        std::vector<std::byte> SampleBuffer;
        
        // The index of the last sample frame written to the buffer. Whole number because the sample buffer follows the master format.
        int MasterFrameIndex;

        // Validates that output formats are supported by the output handler.
        bool ValidateFormat(const OutputFormat &format);

        // Validates that the passed sample frame is valid for the master format.
        bool ValidateSampleFrame(const std::vector<std::byte> &frame);

        /*
            This will be the main output handling thread loop.

            I envision this working in two parts. We're processing each consumer in-between writes, and invoking a function to pass the
            samples in the proper format to the drivers. After this we call a "polling" function. The driver determines if it has what it needs
            to pass the samples along, and if so it does.

            Consuming will be a pipeline. The first step is to actually retrieve the sample frame from the buffer.
            If the index is a whole number this is simple, we just grab it. If it's not we need to grab both frames
            it sits between. After retrieval we'll convert the sample frame(s) to an intermediate representation, 
            likely a 32-bit float between zero and one since this makes for a precise and easy multiplier.

            If we retrieved two sample frames we interpolate between the two, using the index % 1, to produce a single
            normalized sample frame. If the frame does not have the same amount of channels as the output format we
            need to perform channel mapping (the math for which I have not yet figured out).

            Finally we'll map the normalized values to the specified bit-depth, packing it into a raw uint32_t word, 
            and convert it back into a byte vector. This will be passed to the driver with some indication of which
            device it is for.

            The index will be incremented by a pre-calculated amount. This amount is the difference between the source
            and destination sample rates. State will be stored in some kind of struct I have not yet implemented. We'll
            call it "OutputDevice" or something.
        */
        void Update();

    public:

        // Device management. Defer to registered drivers.
        void EnumerateDevices();
        void EnableDevice();
        void DisableDevice();

        // Called by the synth engine, writes samples matching the master parameters to the bufffer.
        void WriteSampleFrame(const std::vector<std::byte> &frame);

        // Sets the master format, reallocating the sample buffer.
        void SetMasterFormat(OutputFormat master_format);
        
        // Constructs an output handler with the given master format.
        OutputHandler(OutputFormat master_format);
};