
#include "stdafx.h"
#include "CamRaphael.h"

#include "Helpers.h"

// Some extra info about the functions...
//
// Camera_Init should be called like:
//     DWORD p1=0x00000001;
//    DWORD p2=0x00000000;
//    Camera_Init(p1, &p2);
//
// Camera_FlashLight can be called with a few values:
//  1 = Enable normal flashlight
//  2 = Disable normal flashlight
//  3 = Enable bright flashlight (auto-disables after about 500ms by the DLL)
//  4 = Reset bright flashlight (this must be called after enabling bright light to reset the internal state)

CamRaphael::CamRaphael()
{
    BrightThreadRunning = false;
    BrightModeResetTimeInMs = 500;
    hBrightThread = NULL;
}

CamRaphael::~CamRaphael()
{
}

bool CamRaphael::Initialize()
{
    // Try to load the library
    hCamera=LoadLibrary(L"HTCCamera1.dll");
    if (!hCamera)
    {
        logger("CamRaphael::Initialize - Failed loading HTCCamera1.dll");
        return false;
    }

    // Try to get the function addresses
    Camera_Init=(CAMINIT)GetProcAddress(hCamera, L"Camera_Init");
    Camera_Deinit=(CAMDEINIT)GetProcAddress(hCamera, L"Camera_Deinit");
    Camera_FlashLight=(CAMFLASH)GetProcAddress(hCamera, L"Camera_FlashLight");
    if (!Camera_Init || !Camera_Deinit || !Camera_FlashLight)
    {
        logger("CamRaphael::Initialize - Failed finding procs for Camera_Init, Camera_Deinit and/or Camera_FlashLight");
        FreeLibrary(hCamera);
        return false;
    }

    // Initialize the camera
    DWORD p1=0x00000001;
    DWORD p2=0x00000000;
    Camera_Init(p1, &p2);
    if (p2 != 0)
    {
        logger("CamRaphael::Initialize - Failed initializing the camera. Is it already running?");
        FreeLibrary(hCamera);
        return false;
    }

    // Done
    return true;
}

void CamRaphael::Deinitialize()
{
    // Disable the light before we close, just in case it's still on
    SetFlashMode(LED_OFF);

    // Unload the camera libary
    FreeLibrary(hCamera);
}

bool CamRaphael::SetFlashMode(FlashModes mode)
{
    BrightStop();

    switch (mode)
    {
    case LED_OFF:
        logger("CamRaphael::SetFlashMode - Setting mode to LED_OFF");
        Camera_FlashLight(2);
        return true;
    case LED_NORMAL:
        logger("CamRaphael::SetFlashMode - Setting mode to LED_NORMAL");
        Camera_FlashLight(1);
        return true;
    case LED_BRIGHT:
        logger("CamRaphael::SetFlashMode - Setting mode to LED_BRIGHT");
        BrightStart();
        return true;
    default:
        break;
    }
    return false;
}

void CamRaphael::BrightStart()
{
    if (!hBrightThread)
    {
        logger("CamRaphael::BrightStart - Starting the thread to keep bright enabled");
        BrightThreadRunning = true;
        hBrightThread = CreateThread(NULL, 0, CamRaphael::BrightThreadStart, this, 0, NULL);
        SetThreadPriority(hBrightThread, THREAD_PRIORITY_HIGHEST);
    }
    else
    {
        logger("CamRaphael::BrightStart - The thread for bright mode was already active, ignoring call");
    }
}

void CamRaphael::BrightStop()
{
    if (hBrightThread)
    {
        logger("CamRaphael::BrightStop - Stopping the bright mode thread");
        BrightThreadRunning = false;
        WaitForSingleObject(hBrightThread, INFINITE);
        hBrightThread = NULL;
    }
    else
    {
        logger("CamRaphael::BrightStop - The thread for bright mode was already stopped, ignoring call");
    }
}

/*static*/ DWORD CamRaphael::BrightThreadStart(LPVOID param)
{
    CamRaphael *instance = (CamRaphael*)param;

    while (instance->BrightThreadRunning)
    {
        // Reset the bright mode (we do this as close as possible to enabling, as timing is essential to minimize flickering)
        instance->Camera_FlashLight(4);

        // Enable bright mode
        instance->Camera_FlashLight(3);

        // Sleep a little
        Sleep(instance->BrightModeResetTimeInMs);
    }

    // Reset flashlight when we break the loop
    instance->Camera_FlashLight(4);

    return 0;
}

/**

        // Bright mode
        logger("Running in bright mode...");
        DWORD dwRunning = 0;
        while (dwRunning < flashlightTimeout * 1000)
        {
            // Reset the bright mode (we do this as close as possible to enabling, as timing is essential to minimize flickering)
            Camera_FlashLight(4);

            // Enable bright mode
            Camera_FlashLight(3);

            // Check if event has been triggered
            if (IsEventSet(hEvent))
                break;

            // Sleep a little
            Sleep(brightResetTimeInMs);

            // Well, this all took about 750 ms, probably a bit more. Who cares - we don't have to be top notch correct.
            dwRunning += brightResetTimeInMs;
        }

        // Reset flashlight when we break the loop
        Camera_FlashLight(4);
*/
