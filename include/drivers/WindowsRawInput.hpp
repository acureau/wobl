/*
    Driver Type: Input
    Driver Target: Windows Raw Input API
*/

#ifdef PLATFORM_WINDOWS
#pragma once

#include <Windows.h>

class WindowsRawInput /*: InputDriver*/
{
    private:
        

    public:
        void Initialize();
};

#endif