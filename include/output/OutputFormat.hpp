enum SampleType
{
    FloatingPoint,
    Integer
};

struct OutputFormat
{
    int SampleRate;
    int BitsPerSample;
    SampleType SampleType;
    int ChannelCount;
};