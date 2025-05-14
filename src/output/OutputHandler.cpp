#include <iostream>
#include "OutputHandler.hpp"

#ifdef PLATFORM_WINDOWS
#include <WindowsWasapi.hpp>
#endif

#ifdef PLATFORM_LINUX
#include "LinuxAlsa.hpp"
#endif 

bool OutputHandler::ValidateFormat(const OutputFormat &format)
{
    if (format.BitsPerSample % 8 > 0)
    {
        std::cerr << "Unsupported bits per sample '" << format.BitsPerSample << "'. Is not a multiple of 8." << std::endl;
        return false;
    }

    if (format.BitsPerSample > 32 || format.BitsPerSample < 8)
    {
        std::cerr << "Unsupported bits per sample '" << format.BitsPerSample << "'. Is not between 8 and 32." << std::endl;
        return false;
    }

    if (format.Type == SampleType::FloatingPoint && format.Sign == SampleSign::Unsigned)
    {
        std::cerr << "Floating point samples cannot be unsigned." << std::endl;
        return false;
    }

    if (format.SampleRate <= 0)
    {
        std::cerr << "Sample rate must be greater than zero." << std::endl;
        return false;
    }

    if (format.ChannelCount <= 0)
    {
        std::cerr << "Channel count must be greater than zero." << std::endl;
        return false;
    }

    return true;
}

bool OutputHandler::ValidateSampleFrame(const std::vector<std::byte> &frame)
{
    int bytes_per_frame = (this->MasterFormat.ChannelCount * this->MasterFormat.BitsPerSample) / 8;
    if (frame.size() != bytes_per_frame)
    {
        std::cerr << "Sample frame size does not match master format." << std::endl;
        return false;
    }
    return true;
}

std::vector<std::byte> InterpolateSampleFrame(float index)
{

}

std::vector<float> NormalizeSampleFrame(const std::vector<std::byte> &frame)
{
    
}

std::vector<std::byte> OutputHandler::GetSampleFrame(const std::string& driver_id, const std::string& device_id)
{
    std::vector<std::byte> sample_frame;

    // Get device state from map.
    DeviceState &device_state = this->DeviceIDStateMap.at(driver_id + "::" + device_id);

    // Determine bytes per master sample frame.
    int master_bytes_per_frame = (this->MasterFormat.BitsPerSample * this->MasterFormat.ChannelCount) / 8;

    // TODO: USE THE CURRENT INDEX TO INTERPOLATE THE SAMPLE FRAME
    
    
    // TODO: WE INCREMENT AT THE END, AND ONLY IF PERMISSIBLE
    // Calculate device state index increment. Determine if we can increment.
    // e.g. Producer: 44100, Consumer: 44100, Increment: 1.0
    // e.g. Producer: 44100, Consumer: 22050, Increment: 2.0
    // e.g. Producer: 44100, Consumer: 66150, Increment: 0.75
    float device_state_index_increment = (float)this->MasterFormat.SampleRate / device_state.second.SampleRate;
    if (device_state.first + device_state_index_increment <= this->MasterFrameIndex)
    {
        // Increment index.
    }

    return std::vector<std::byte>();
}

void OutputHandler::RegisterDriver(std::unique_ptr<OutputDriver> driver)
{
    // Acquire driver list lock.
    std::lock_guard<std::mutex> driver_lock(this->DriverMutex);

    // Add to the active driver map.
    this->RegisteredOutputDrivers.emplace(driver->Id, std::move(driver));
}

void OutputHandler::Update()
{
    // Iterate over all drivers.
    for (auto& [_, driver] : this->RegisteredOutputDrivers)
    {
        // Invoke driver flush method.
        driver->Flush();
    }
}

std::vector<OutputDevice> OutputHandler::EnumerateDevices()
{
    std::vector<OutputDevice> devices;
    
    // Acquire driver list lock.
    std::lock_guard<std::mutex> driver_lock(this->DriverMutex);

    // Iterate over all drivers.
    for (auto& [_, driver] : this->RegisteredOutputDrivers)
    {
        // Enumerate devices from driver, add them to the top-level list.
        std::vector<OutputDevice> driver_devices = driver->EnumerateDevices();
        devices.insert(devices.end(), driver_devices.begin(), driver_devices.end());
    }

    return devices;
}

void OutputHandler::EnableDevice(const OutputDevice& device)
{
    // Acquire driver list lock.
    std::lock_guard<std::mutex> driver_lock(this->DriverMutex);

    // Pass the call down to the driver, retrieve device output format.
    OutputFormat device_format = this->RegisteredOutputDrivers.at(device.DriverId)->EnableDevice(device, this->MasterFormat);

    // Add the device to the state map.
    DeviceState initial_device_state = {0.0f, device_format};
    this->DeviceIDStateMap.emplace(device.DriverId + "::" + device.DeviceId, initial_device_state);
}

void OutputHandler::DisableDevice(const OutputDevice& device)
{
    // Acquire driver list lock.
    std::lock_guard<std::mutex> driver_lock(this->DriverMutex);

    // Pass the call down to the driver.
    this->RegisteredOutputDrivers.at(device.DriverId)->DisableDevice(device);

    // Remove the device from the state map.
    this->DeviceIDStateMap.erase(device.DriverId + "::" + device.DeviceId);
}

void OutputHandler::WriteSampleFrame(const std::vector<std::byte> &frame)
{
    // Validate sample frame.
    if (!ValidateSampleFrame(frame))
    {
        return;
    }

    // Determine bytes per frame.
    int bytes_per_frame = (this->MasterFormat.BitsPerSample * this->MasterFormat.ChannelCount) / 8;

    // Write sample frame to buffer.
    std::copy(frame.begin(), frame.end(), this->SampleBuffer.begin() + (this->MasterFrameIndex * bytes_per_frame));

    // Increment the frame index.
    this->MasterFrameIndex = (this->MasterFrameIndex + 1) % (this->SampleBuffer.size() / bytes_per_frame);
}

void OutputHandler::SetMasterFormat(OutputFormat master_format)
{
    // Validate passed format.
    if (!this->ValidateFormat(master_format))
    {
        std::cerr << "Unable to set master format." << std::endl;
        return;
    }

    // Set master format.
    this->MasterFormat = master_format;

    // Determine frame count for 10ms sample buffer.
    int frame_count = master_format.SampleRate / 100;
    
    // Determine bytes per frame.
    int bytes_per_frame = (master_format.BitsPerSample * master_format.ChannelCount) / 8;
    
    // Resize sample buffer.
    this->SampleBuffer.resize(frame_count * bytes_per_frame);

    // Reset frame index.
    this->MasterFrameIndex = 0;
}

OutputHandler::OutputHandler(OutputFormat master_format)
{
    // Set the master format.
    this->SetMasterFormat(master_format);

    // Register platform specific drivers.
    #ifdef PLATFORM_WINDOWS
    this->RegisterDriver(std::make_unique<WindowsWasapi>());
    #endif
    #ifdef PLATFORM_LINUX
    this->RegisterDriver(std::make_unique<LinuxAlsa>());
    #endif

    // Spawn polling thread.
    this->PollingThreadActive = true;
    this->PollingThread = std::thread([this]() {

        // Initialize all drivers.
        for (auto& [_, driver] : this->RegisteredOutputDrivers)
        {
            driver->Initialize
            (
                std::make_shared<SampleCallback>
                (
                    [this](const std::string& driver_id, const std::string& device_id) -> std::vector<std::byte>
                    {
                        return this->GetSampleFrame(driver_id, device_id);
                    }
                )
            );
        }

        // Poll all drivers.
        while (this->PollingThreadActive) {
            this->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

OutputHandler::~OutputHandler()
{
    // Clean up polling thread.
    this->PollingThreadActive = false;
    if (this->PollingThread.joinable())
    {
        this->PollingThread.join();
    }
}

/*
    UNSORTED SCRATCH CODE
*/

uint32_t BytesToWord(std::vector<std::byte> bytes)
{
    uint32_t word = 0;
    for (int byte_index = 0; byte_index < bytes.size(); byte_index++)
    {
        word |= static_cast<uint32_t>(bytes[byte_index]) << (byte_index * 8);
    }
    return word;
}

float NormalizeSample(uint32_t sample, OutputFormat format)
{
    return 0.0f;
}

uint32_t MapNormal(float normal, OutputFormat format)
{
    return 0;
}

std::vector<std::byte> WordToBytes(uint32_t word)
{
    return std::vector<std::byte>();
}

std::vector<std::byte> ConvertSamples(const std::vector<std::byte> &samples, const OutputFormat &source_format, const OutputFormat &destination_format)
{
    std::vector<std::byte> converted_samples;

    // Calculate bytes per sample and channel.
    int bytes_per_sample = source_format.BitsPerSample / 8;
    int bytes_per_frame = bytes_per_sample * source_format.ChannelCount;

    // Iterate over all sample frames.
    for (int frame_index = 0; frame_index < samples.size(); frame_index += bytes_per_frame)
    {
        // Iterate over samples in sample frame.
        for (int sample_index = 0; sample_index < bytes_per_frame; sample_index += bytes_per_sample)
        {
            // Get the sample from the samples vector.
            int sample_offset = frame_index + sample_index;
            std::vector<std::byte> sample_bytes(samples.begin() + sample_offset, samples.begin() + sample_offset + bytes_per_sample);

            // Convert sample bytes to word.
            uint32_t raw_sample = BytesToWord(sample_bytes);

            // Normalize sample.
            float normalized_sample = NormalizeSample(raw_sample, source_format);

            // Map normalized sample.
            uint32_t mapped_sample = MapNormal(normalized_sample, destination_format);

            // One to determine the "range" of a data type from the output format. Or really, the "position in the range" as a percentage.
            // One to produce a uint32_t given an output format and this "position in the range".
        }
    }

    return converted_samples;
}