
#pragma once

#include "stdafx.h"
#include "ICamDevice.h"
#include <windows.h>

// HTCCamera1.dll API functions
typedef int (__stdcall *CAMINIT)(DWORD dw1, DWORD *dw2);
typedef int (__stdcall *CAMDEINIT)();
typedef int (__stdcall *CAMFLASH)(DWORD dwValue);

class CamRaphael : implements ICamDevice
{
public:
    CamRaphael();
    ~CamRaphael();

public:
    bool Initialize();
    bool SetFlashMode(FlashModes mode);
    void Deinitialize();
    int BrightModeResetTimeInMs;

private:
    void BrightStart();
    void BrightStop();
    static DWORD BrightThreadStart(LPVOID);
    bool BrightThreadRunning;

    HMODULE hCamera;
    CAMINIT Camera_Init;
    CAMDEINIT Camera_Deinit;
    CAMFLASH Camera_FlashLight;
    HANDLE hBrightThread;
};
