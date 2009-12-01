
#pragma once

#include "stdafx.h"
#include "ICamDevice.h"
#include <windows.h>

// API functions
typedef int (__stdcall *LEO_INIT)();
typedef int (__stdcall *LEO_SETCURRENT)(DWORD dw1);

class CamLeo : implements ICamDevice
{
public:
    CamLeo();
    ~CamLeo();

public: // methods
    bool Initialize();
    bool SetFlashMode(FlashModes mode);
    void Deinitialize();

public: // variables
    int BrightModeResetTimeInMs;

private: // methods
    void BrightStart();
    void BrightStop();
    static DWORD BrightThreadStart(LPVOID);
    void BrightViaApi();
    void BrightViaIo(HANDLE hDevice);

private: // variables
    bool BrightThreadRunning;
    HMODULE hCamera;
    LEO_INIT InitFlashLight;
    LEO_SETCURRENT PMICFlashLED_SetCurrent;
    HANDLE hBrightThread;
    HANDLE hBrightEvent;
};
