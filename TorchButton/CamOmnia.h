
#pragma once

#include "stdafx.h"
#include "ICamDevice.h"
#include <windows.h>

class CamOmnia : implements ICamDevice
{
public:
    CamOmnia();
    ~CamOmnia();

public:
    bool Initialize();
    bool SetFlashMode(FlashModes mode);
    void Deinitialize();

private:
    void SetFlashlight(bool on);
    HANDLE hCam;
};
