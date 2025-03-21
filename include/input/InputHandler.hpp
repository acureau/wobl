#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <unordered_map>
#include "Input.hpp"
#include "InputDevice.hpp"
#include "InputDriver.hpp"

using StringPairVector = std::vector<std::pair<std::string, std::string>>;

/*
    We should move the devices to their parent drivers. We can access them without recreating them after all.
    But then how will we determine which drivers are active? How will this interface take device IDs?
    Maybe we need a map of device ids to driver ids?

    That makes sense I guess, the only thing this class should manage is drivers. It should be a generic interface to those drivers,
    and invoke updates in them. It just needs to expose the means of browsing, selecting, and accessing the data of devices in a
    driver agnostic way.

    We're nearly there now but I need a break. I have been thinking about abstract input interfaces for too long. This will be a
    damn good system though. I'll be able to wire up whatever controls I want to my synth.
*/

class InputHandler
{
    private:
        std::unordered_map<std::string, InputDriver> RegisteredDrivers;
        std::unordered_map<std::string, InputDevice> ActiveDevices;

        void InvokeCallbacks(const StringPairVector &updated_inputs);

        // Keep devices up to date, main thread loop.
        void Update();

    public:
        // ID / name enumeration.
        StringPairVector EnumerateDevices();
        StringPairVector EnumerateInputs(std::string device_id);
        
        // Device activation / deactivation.
        void ActivateDevice(std::string device_id);
        void DeactivateDevice(std::string device_id);

        // Polling.
        template<typename T>
        T GetInputState(std::string device_id, std::string input_id);

        // Callback.
        void AddInputChangedCallback(int update_interval_ms, std::function<void(const StringPairVector &)> callback);

        // Handler initialization.
        void RegisterDriver(InputDriver driver);
        InputHandler();
};