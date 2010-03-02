
#include "stdafx.h"
#include "CamOmnia.h"

#include "Helpers.h"

CamOmnia::CamOmnia()
{
    hCam = 0;
}

CamOmnia::~CamOmnia()
{
}

bool CamOmnia::Initialize()
{
    // Get the OEM string
    wchar_t oemInfo[256];
    SystemParametersInfo(SPI_GETOEMINFO, 256, oemInfo, 0);

    // Samsung Omnia's have SGH or SCH (Samsung GSM/CDMA Handset) prefix. We (hopefully) support i9xx series.
    if (wcsstr(oemInfo, L"SGH-i9") == NULL &&
        wcsstr(oemInfo, L"SCH-i9") == NULL)
    {
        // Not a supported device
        logger("CamOmnia::Initialize - Failed OEM info check, does not contain SGH-i9 or SCH-i9");
        return false;
    }

    // Open the CAM1: device
    hCam = CreateFile(
        L"CAM1:",
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    // Check for error
    if (hCam == INVALID_HANDLE_VALUE)
    {
        // Could not initialize
        logger("CamOmnia::Initialize - Failed to open CAM1: device");
        hCam = 0;
        return false;
    }

    // Success
    return true;
}

void CamOmnia::Deinitialize()
{
    // Disable the light before we close, just in case it's still on
    SetFlashMode(LED_OFF);

    // Close the handle
    if (hCam)
    {
        CloseHandle(hCam);
        hCam = 0;
    }
}

bool CamOmnia::SetFlashMode(FlashModes mode)
{
    switch (mode)
    {
    case LED_OFF:
        logger("CamOmnia::SetFlashMode - Setting mode to LED_OFF");
        SetFlashlight(false);
        return true;
    case LED_NORMAL:
        logger("CamOmnia::SetFlashMode - Setting mode to LED_NORMAL");
        SetFlashlight(true);
        return true;
    case LED_BRIGHT:
        logger("CamOmnia::SetFlashMode - LED_BRIGHT mode not supported!");
        return false;
    default:
        break;
    }
    return false;
}

void CamOmnia::SetFlashlight(bool on)
{
    // Must have handle
    if (!hCam)
    {
        return;
    }

    // Determine on/off
    unsigned char signal = on ? 1 : 0;

    // Execute operation
    DeviceIoControl(hCam, 0x777, (LPVOID)&signal, 1, 0, 0, 0, 0);
}