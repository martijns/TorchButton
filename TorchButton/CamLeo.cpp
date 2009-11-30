
#include "stdafx.h"
#include "CamLeo.h"

#include "Helpers.h"

CamLeo::CamLeo()
{
    BrightThreadRunning = false;
    BrightModeResetTimeInMs = 500;
    hBrightThread = NULL;
}

CamLeo::~CamLeo()
{
}

bool CamLeo::Initialize()
{
    // Try to load the library
    hCamera=LoadLibrary(L"CameraPlatform.dll");
    if (!hCamera)
    {
        logger("CamLeo::Initialize - Couldn't find or load CameraPlatform.dll");
        return false;
    }

    // Try to get the function addresses
    InitFlashLight = (LEO_INIT) GetProcAddress(hCamera, L"InitFlashLight");
    PMICFlashLED_SetCurrent = (LEO_SETCURRENT) GetProcAddress(hCamera, L"PMICFlashLED_SetCurrent");
    if (!InitFlashLight || !PMICFlashLED_SetCurrent)
    {
        logger("CamLeo::Initialize - Couldn't find functions InitFlashLight and/or PMICFlashLED_SetCurrent");
        FreeLibrary(hCamera);
        return false;
    }

    // Initialize the camera
    InitFlashLight();

    // Done
    return true;
}

void CamLeo::Deinitialize()
{
    // Disable the light before we close, just in case it's still on
    SetFlashMode(LED_OFF);

    // Unload the camera libary
    FreeLibrary(hCamera);
}

bool CamLeo::SetFlashMode(FlashModes mode)
{
    BrightStop();

    switch (mode)
    {
    case LED_OFF:
        logger("CamLeo::SetFlashMode - Setting mode LED_OFF");
        PMICFlashLED_SetCurrent(0);
        return true;
    case LED_NORMAL:
        logger("CamLeo::SetFlashMode - Setting mode LED_NORMAL");
        PMICFlashLED_SetCurrent(1);
        return true;
    case LED_BRIGHT:
        logger("CamLeo::SetFlashMode - Setting mode LED_BRIGHT");
        BrightStart();
        return true;
    default:
        break;
    }
    return false;
}

void CamLeo::BrightStart()
{
    if (!hBrightThread)
    {
        logger("CamLeo::BrightStart - Starting the thread to keep bright enabled");
        BrightThreadRunning = true;
        hBrightThread = CreateThread(NULL, 0, CamLeo::BrightThreadStart, this, 0, NULL);
        SetThreadPriority(hBrightThread, THREAD_PRIORITY_HIGHEST);
    }
    else
    {
        logger("CamLeo::BrightStart - Thread for bright mode was already running, ignoring call");
    }
}

void CamLeo::BrightStop()
{
    if (hBrightThread)
    {
        logger("CamLeo::BrightStop - Stopping the thread for bright mode");
        BrightThreadRunning = false;
        WaitForSingleObject(hBrightThread, INFINITE);
        hBrightThread = NULL;
    }
    else
    {
        logger("CamLeo::BrightStop - Thread for bright mode was already stopped, ignoring call");
    }
}

/*static*/ DWORD CamLeo::BrightThreadStart(LPVOID param)
{
    CamLeo *instance = (CamLeo*)param;

    // See if we can open the GIO1: device
    HANDLE hDevice = CreateFileW(L"GIO1:", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        logger("CamLeo::BrightThreadStart - Successfully opened GIO1: device, using bright mode via DeviceIoControl for more stable bright mode");
        instance->BrightViaIo(hDevice);
        CloseHandle(hDevice);
    }
    else
    {
        logger("CamLeo::BrightThreadStart - Failed opening GIO1: device, falling back to bright mode via API (might result in more flickering)");
        instance->BrightViaApi();
    }

    return 0;
}

void CamLeo::BrightViaApi()
{
    while (BrightThreadRunning)
    {
        // Reset the bright mode (we do this as close as possible to enabling, as timing is essential to minimize flickering)
        PMICFlashLED_SetCurrent(0);

        // Enable bright mode
        PMICFlashLED_SetCurrent(31);

        // Sleep a little
        Sleep(BrightModeResetTimeInMs);
    }

    // Reset flashlight when we break the loop
    PMICFlashLED_SetCurrent(0);
}

// We use the InitFlashLight() for now, but in case we ever need to also do that via DeviceIo:
//inVal = 0x9f;
//memset(bOutBuffer, 9000, sizeof(BYTE));
//bResult = DeviceIoControl(hDevice, 0x220000 | 0xc10, &inVal, sizeof(LONGLONG), bOutBuffer, 9000, &lVal, NULL); // init
//
//inVal = 0x8f;
//memset(bOutBuffer, 9000, sizeof(BYTE));
//bResult = DeviceIoControl(hDevice, 0x220000 | 0xc10, &inVal, sizeof(LONGLONG), bOutBuffer, 9000, &lVal, NULL); // init
//
//bOutBuffer can probably be a lot smaller. A DWORD is probably enough, like the other IO's.

void CamLeo::BrightViaIo(HANDLE hDevice)
{
    DWORD dOutSize = 0;
    DWORD dBrightLed = 0x8f;
    DWORD dResult = 0;
    BOOL bResult;

    while (BrightThreadRunning)
    {
        // Reset the bright mode (we do this as close as possible to enabling, as timing is essential to minimize flickering)
        bResult = DeviceIoControl(hDevice, 0x220000 | 0xc04, &dBrightLed, sizeof(DWORD), &dResult, sizeof(DWORD), &dOutSize, NULL); // off

        // Enable bright mode
        bResult = DeviceIoControl(hDevice, 0x220000 | 0xc00, &dBrightLed, sizeof(DWORD), &dResult, sizeof(DWORD), &dOutSize, NULL); // on

        // Sleep a little
        Sleep(BrightModeResetTimeInMs);
    }

    // Reset flashlight when we break the loop
    bResult = DeviceIoControl(hDevice, 0x220000 | 0xc04, &dBrightLed, sizeof(DWORD), &dResult, sizeof(DWORD), &dOutSize, NULL); // off
}
