#pragma once

#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "Stream.hpp"

class WASAPI : public Stream
{
    private:
        IMMDeviceEnumerator* DeviceEnumerator = nullptr;
        IMMDeviceCollection* DeviceCollection = nullptr;
        IMMDevice* ActiveDevice = nullptr;
        DeviceParams ActiveDeviceParams;
        IAudioClient* AudioClient = nullptr;
        WAVEFORMATEX* WaveFormat = nullptr;
        IAudioRenderClient* RenderClient = nullptr;
        HANDLE RenderEvent = NULL;
        std::thread RenderThread;
        std::atomic<bool> Playing { false };

        std::string WideStringToString(LPWSTR lp_wide_string)
        {
            std::wstring wide_string(lp_wide_string);
            std::string string(wide_string.begin(), wide_string.end());
            return string;
        }

        std::string GetDeviceID(IMMDevice* device)
        {
            std::string device_id = "";
            
            LPWSTR wide_device_id;
            if(device->GetId(&wide_device_id) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get device identifier." << std::endl;
            }
            else 
            {
                device_id = WideStringToString(wide_device_id);
            }
            CoTaskMemFree(wide_device_id);
            
            return device_id;
        }

        std::string GetDeviceName(IMMDevice* device)
        {
            std::string device_name = "";

            IPropertyStore* property_store;
            if (device->OpenPropertyStore(STGM_READ, &property_store) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get device property store." << std::endl;
            }
            else
            {
                PROPVARIANT device_name_property;
                if (property_store->GetValue(PKEY_Device_FriendlyName, &device_name_property) != S_OK)
                {
                    std::cerr << "WASAPI: Failed to get device name property." << std::endl;
                }
                else
                {
                    device_name = WideStringToString(device_name_property.pwszVal);
                }
                PropVariantClear(&device_name_property);
            }
            property_store->Release();
            
            return device_name;
        }

        std::vector<IMMDevice*> EnumerateDevices()
        {
            std::vector<IMMDevice*> devices;

            // Ensure enumerator is initialized.
            if (!DeviceEnumerator)
            {
                std::cerr << "WASAPI: Could not enumerate devices." << std::endl;
                return devices;
            }

            // Enumerate active output devices.
            if (DeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &DeviceCollection) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get device collection." << std::endl;
            }
            
            // Count devices.
            UINT device_count;
            if (DeviceCollection->GetCount(&device_count) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get device count." << std::endl;
            }

            // Iterate over all devices, adding them to the vector.
            for (UINT i = 0; i < device_count; i++)
            {
                IMMDevice* device;
                if (DeviceCollection->Item(i, &device) != S_OK)
                {
                    std::cerr << "WASAPI: Failed to get device by index." << std::endl;
                }
                else
                {
                    devices.push_back(device);
                }
            }

            return devices;
        }

        void Play(std::function<std::vector<float>(DeviceParams, int)> callback)
        {
            // Get the size of the audio buffer.
            UINT32 buffer_frame_count;
            if (AudioClient->GetBufferSize(&buffer_frame_count) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get buffer size." << std::endl;
                return;
            }

            // Populate buffer with samples.
            auto PopulateBuffer = [this, callback, buffer_frame_count]
            {
                // Get empty buffer.
                BYTE* sample_buffer;
                this->RenderClient->GetBuffer(buffer_frame_count, &sample_buffer);

                // Populate buffer.
                std::vector<float> samples = callback(this->ActiveDeviceParams, buffer_frame_count);
                memcpy(sample_buffer, samples.data(), samples.size() * sizeof(float));

                // Release buffer for playback.
                this->RenderClient->ReleaseBuffer(buffer_frame_count, NULL);
            };

            AudioClient->Start();
            PopulateBuffer();

            // Event wait loop.
            while (Playing)
            {
                if (WaitForSingleObject(RenderEvent, 2000) != WAIT_OBJECT_0)
                {
                    std::cerr << "WASAPI: Render event timed out." << std::endl;
                    AudioClient->Stop();
                    break;
                }
                else
                {
                    PopulateBuffer();
                }
            }
        }

    public:
        std::vector<DeviceIdentity> GetDevices() override 
        {
            std::vector<DeviceIdentity> device_identities;
            for (IMMDevice* device : EnumerateDevices())
            {
                std::string device_id = GetDeviceID(device);
                std::string device_name = GetDeviceName(device);
                if (device_id != "" && device_name != "")
                {
                    device_identities.emplace_back(device_id, device_name);
                }
                device->Release();
            }
            return device_identities;
        }

        bool SetDevice(DeviceIdentity device_identity) override
        {
            for (IMMDevice* device : EnumerateDevices())
            {
                if (GetDeviceID(device) == device_identity.Identifier)
                {
                    ActiveDevice = device;
                    return true;
                }
                else 
                {
                    device->Release();
                }
            }
            return false;
        }

        bool Initialize() override
        {
            // Ensure device is selected.
            if (!ActiveDevice)
            {
                std::cerr << "WASAPI: Cannot configure before selecting device." << std::endl;
                return false;
            }

            // Initialize AudioClient.
            HRESULT result = ActiveDevice->Activate
            (
                __uuidof(IAudioClient),
                CLSCTX_ALL,
                NULL,
                (void**)&AudioClient
            );

            if (result != S_OK)
            {
                std::cerr << "WASAPI: Failed to create AudioClient." << std::endl;
                return false;
            }

            // Get device wave format.
            if (AudioClient->GetMixFormat(&WaveFormat) != S_OK)
            {
                std::cerr << "WASAPI: Failed to get device wave format." << std::endl;
                return false;
            }

            // Populate device params.
            ActiveDeviceParams.SampleRate = static_cast<int>(WaveFormat->nSamplesPerSec);
            ActiveDeviceParams.ChannelCount = static_cast<int>(WaveFormat->nChannels);

            // Get device period.
            REFERENCE_TIME device_period;
            if (AudioClient->GetDevicePeriod(NULL, &device_period) != S_OK)
            {
                std::cerr << "WASAPI: Failed to retrieve device period." << std::endl;
                return false;
            }

            // Initialize AudioClient.
            result = AudioClient->Initialize
            (
                AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                device_period,
                0,
                WaveFormat,
                NULL
            );

            if (result != S_OK)
            {   
                std::cerr << "WASAPI: Failed to initialize device." << std::endl;
                return false;
            }

            // Initialize event handler.
            RenderEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (RenderEvent == NULL)
            {
                std::cerr << "WASAPI: Failed to create render event handle." << std::endl;
                return false;
            }

            if (AudioClient->SetEventHandle(RenderEvent) != S_OK)
            {
                std::cerr << "WASAPI: Failed to set render event handle." << std::endl;
                return false;
            }

            // Initialize RenderClient.
            result = AudioClient->GetService
            (
                __uuidof(IAudioRenderClient),
                (void**)&RenderClient
            );

            if (result != S_OK)
            {
                std::cerr << "WASAPI: Failed to create RenderClient." << std::endl;
                return false;
            }

            return true;
        }

        bool Start(std::function<std::vector<float>(DeviceParams, int)> callback) override
        {
            if (!Playing.load())
            {
                Playing = true;
                RenderThread = std::thread([this, callback]() 
                {
                    this->Play(callback);
                });
                return true;
            }
            else
            {
                std::cerr << "WASAPI: Playback already started." << std::endl;
                return false;
            }
        }

        bool Stop() override
        {
            if (Playing.load())
            {
                Playing = false;
                if (RenderThread.joinable())
                {
                    RenderThread.join();
                }
                AudioClient->Stop();
                return true;
            }
            else
            {
                std::cerr << "WASAPI: Playback not active." << std::endl;
                return false;
            }
        }

        WASAPI()
        {
            // Initialize the COM library.
            CoInitialize(NULL);

            // Initialize MMDeviceEnumerator.
            HRESULT result = CoCreateInstance
            (
                __uuidof(MMDeviceEnumerator),
                NULL,
                CLSCTX_ALL,
                __uuidof(IMMDeviceEnumerator),
                (LPVOID*)&DeviceEnumerator
            );

            if (result != S_OK) 
            {
                std::cerr << "WASAPI: Failed to create device enumerator." << std::endl;
            }
        }

        ~WASAPI()
        {
            // Release COM objects.
            if (DeviceEnumerator) DeviceEnumerator->Release();
            if (DeviceCollection) DeviceCollection->Release();
            if (ActiveDevice) ActiveDevice->Release();
            if (AudioClient) AudioClient->Release();
            if (RenderClient) RenderClient->Release();
            
            // Free COM allocated objects.
            if (WaveFormat) CoTaskMemFree(WaveFormat);

            // Close handles.
            if (RenderEvent != NULL) CloseHandle(RenderEvent);

            // Uninitialize the COM library.
            CoUninitialize();
        }
};
