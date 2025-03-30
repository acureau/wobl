enum SampleType
{
    FloatingPoint,
    Integer
};

class OutputHandler
{
    int MasterSampleRate;
    int MasterBitsPerSample;
    SampleType MasterSampleType;

    virtual ~OutputHandler() = default;
};