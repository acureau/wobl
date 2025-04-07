#pragma once

enum SampleType
{
    FloatingPoint,
    Integer
};

enum SampleSign
{
    Signed,
    Unsigned
};

struct OutputFormat
{
    int SampleRate;
    int BitsPerSample;
    SampleType Type;
    SampleSign Sign;
    int ChannelCount;
};