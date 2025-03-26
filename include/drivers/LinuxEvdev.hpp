/*
    Driver Type: Input
    Driver Target: Linux Input Subsystem Userspace API
*/

#ifdef PLATFORM_LINUX
#pragma once

class LinuxEvdev /*: InputDriver*/
{
    private:
        

    public:
        void Initialize();
};

#endif