#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include "InputEvent.hpp"
#include "InputDriver.hpp"

// Function signature for input event callbacks.
using InputEventCallback = std::function<void(std::queue<InputEvent>)>;

class InputHandler
{
    private:
        // The main input handling thread.
        std::thread PollingThread;

        // Flag set when the polling thread is running.
        std::atomic<bool> PollingThreadActive;
        
        // Input driver list thread lock.
        std::mutex DriverMutex;

        // A list of registered input drivers.
        std::vector<std::unique_ptr<InputDriver>> RegisteredInputDrivers;
        
        // Callback function list thread lock.
        std::mutex CallbackMutex;

        // A list of registered callback functions.
        std::vector<std::shared_ptr<InputEventCallback>> RegisteredCallbacks;

        // Main thread loop. Polls drivers and invokes callbacks.
        void Update();

        // Method used to register input drivers.
        void RegisterDriver(std::unique_ptr<InputDriver> driver);

    public:
        // Method used to register callback functions.
        void RegisterInputEventCallback(std::shared_ptr<InputEventCallback> callback);

        // Registers drivers, spawns input handler thread.
        InputHandler();
        ~InputHandler();
};