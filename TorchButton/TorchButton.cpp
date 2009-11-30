
#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include <winreg.h>

#include "ICamDevice.h"
#include "CamRaphael.h"
#include "CamLeo.h"

#include "Helpers.h"
#include "RegistrySettings.h"

int _tmain(int argc, _TCHAR* argv[])
{
    logger("Starting TorchButton...");

    // Create a application-unique event, used to determine if the application is already running. If it
    // is, it is used to notify the currently running application.
    logger("Creating event...");
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, EVENT_NAME);
    if (!hEvent)
    {
        logger("Error creating event");
        DisplayError(L"Error creating event.");
        return 1;
    }

    // If we get the already exists error, it means the application is already running. In this case,
    // we simply set the event and get out of here.
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        logger("Application is already running, signalling the event so that it can stop");
        SetEvent(hEvent);
        CloseHandle(hEvent);
        return 0;
    }

    // Default settings
    BOOL bBright = false;
    BOOL bBlink = false;
    BOOL bSos = false;
    BOOL bPtt = false;

    // Check our switches
    for (int i=0; i < argc; i++)
    {
        if (wcscmp(L"/bright", argv[i]) == 0)
        {
            logger("Found argument: /bright"); // don't feel like writing something to convert wchar to char
            bBright = true;
        }
        if (wcscmp(L"/blink", argv[i]) == 0)
        {
            logger("Found argument: /blink");
            bBlink = true;
        }
        if (wcscmp(L"/sos", argv[i]) == 0)
        {
            logger("Found argument: /sos");
            bSos = true;
        }
        if (wcscmp(L"/ptt", argv[i]) == 0)
        {
            logger("Found argument: /ptt");
            bPtt = true;
        }
    }

    // Read settings from the registry
    DWORD flashlightTimeout = 0;
    DWORD blinkTimeOnInMs = 0;
    DWORD blinkTimeOffInMs = 0;
    wchar_t* sosText = new wchar_t[8192];
    DWORD sosUnitTimeInMs = 0;
    DWORD brightResetTimeInMs = 0;
    DWORD brightWarningAccepted = 0;

    HKEY hKey = 0;

    // Open or create the key
    logger("Opening or creating (if nonexisting) the regkey...");
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGKEY_BASE, NULL, NULL, REG_OPTION_NON_VOLATILE, NULL, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        // Read values from registry
        DWORD dwSize;
        dwSize = sizeof(DWORD);
        ReadValue(hKey, REGVALUE_VALUE_FLASHLIGHT_TIMEOUT, REG_DWORD, (BYTE*)&flashlightTimeout, &dwSize, (BYTE*)&REGVALUE_DEFAULT_FLASHLIGHT_TIMEOUT, sizeof(DWORD));
        ReadValue(hKey, REGVALUE_VALUE_BLINK_TIME_ON_IN_MS, REG_DWORD, (BYTE*)&blinkTimeOnInMs, &dwSize, (BYTE*)&REGVALUE_DEFAULT_BLINK_TIME_ON_IN_MS, sizeof(DWORD));
        ReadValue(hKey, REGVALUE_VALUE_BLINK_TIME_OFF_IN_MS, REG_DWORD, (BYTE*)&blinkTimeOffInMs, &dwSize, (BYTE*)&REGVALUE_DEFAULT_BLINK_TIME_OFF_IN_MS, sizeof(DWORD));
        ReadValue(hKey, REGVALUE_VALUE_SOS_UNIT_TIME_IN_MS, REG_DWORD, (BYTE*)&sosUnitTimeInMs, &dwSize, (BYTE*)&REGVALUE_DEFAULT_SOS_UNIT_TIME_IN_MS, sizeof(DWORD));
        ReadValue(hKey, REGVALUE_VALUE_BRIGHT_RESET_TIME_IN_MS, REG_DWORD, (BYTE*)&brightResetTimeInMs, &dwSize, (BYTE*)&REGVALUE_DEFAULT_BRIGHT_RESET_TIME_IN_MS, sizeof(DWORD));
        ReadValue(hKey, REGVALUE_VALUE_BRIGHT_WARNING_ACCEPTED, REG_DWORD, (BYTE*)&brightWarningAccepted, &dwSize, (BYTE*)&REGVALUE_DEFAULT_BRIGHT_WARNING_ACCEPTED, sizeof(DWORD));

        dwSize = 8192 * sizeof(wchar_t);
        ReadValue(hKey, REGVALUE_VALUE_SOS_TEXT, REG_SZ, (BYTE*)sosText, &dwSize, (BYTE*)REGVALUE_DEFAULT_SOS_TEXT, wcslen(REGVALUE_DEFAULT_SOS_TEXT) * sizeof(wchar_t));

        // Check if we need to display the warning for bright mode
        if (bBright && !brightWarningAccepted)
        {
            logger("Using bright mode for the first time, displaying warning message...");
            int res = MessageBox(NULL, L"You are using bright mode for the first time. Be warned that your device was not built to sustain bright mode and that by using this mode, you may damage your device. This warning is only shown once! Do you want to continue at your own risk?", L"WARNING", MB_YESNO);
            if (res == IDYES)
            {
                // Update the registry to save this answer
                logger("User agreed to the warning, saving answer and continuing...");
                DWORD accepted = 1;
                RegSetValueEx(hKey, REGVALUE_VALUE_BRIGHT_WARNING_ACCEPTED, NULL, REG_DWORD, (BYTE*)&accepted, sizeof(DWORD));
            }
            else
            {
                // Shut down
                logger("User didn't agree to the warning, shutting down...");
                RegCloseKey(hKey);
                return 1;
            }
        }

        RegCloseKey(hKey);
    }
    else
    {
        logger("Error opening registry key HKLM\\Software\\TorchButton, aborting...");
        DisplayError(L"Error opening registry key HKLM\\Software\\TorchButton, aborting...");
        return 1;
    }

    // Validate the flashlight timeout
    if (flashlightTimeout < 0)
    {
        logger("The configured timeout was less than 0, setting it to 0...");
        flashlightTimeout = 0;
    }
    if (blinkTimeOnInMs < 0)
    {
        logger("The configured blink time (on) was less than 0, setting it to 0...");
        blinkTimeOnInMs = 0;
    }
    if (blinkTimeOffInMs < 0)
    {
        logger("The configured blink time (off) was less than 0, setting it to 0...");
        blinkTimeOffInMs = 0;
    }
    if (sosUnitTimeInMs < 0)
    {
        logger("The configured sos unit time was less than 0, setting it to 0...");
        sosUnitTimeInMs = 0;
    }
    if (brightResetTimeInMs < 0)
    {
        logger("The configured bright reset time was less than 0, setting it to 0...");
        brightResetTimeInMs = 0;
    }

    // Get a camera device instance
    logger("Trying to initialize for Raphael...");
    ICamDevice *cam = new CamRaphael();
    if (cam->Initialize())
    {
        logger("Initialized camera for Raphael...");
        ((CamRaphael*)cam)->BrightModeResetTimeInMs = brightResetTimeInMs;
    }
    else
    {
        delete cam;
        logger("Failed for Raphael. Trying to initialize for Leo...");
        cam = new CamLeo();
        if (cam->Initialize())
        {
            logger("Initialized camera for Leo...");
            ((CamLeo*)cam)->BrightModeResetTimeInMs = 500;
        }
        else
        {
            logger("Failed for Leo.");
            DisplayError(L"Couldn't detect a supported device, exiting...");
            CloseHandle(hEvent);
            return 1;
        }
    }

    // Reset the event, just in case
    ResetEvent(hEvent);

    if (bBright)
    {
        // Normal mode
        logger("Running in bright mode...");

        logger("Enabling flashlight...");
        if (cam->SetFlashMode(ICamDevice::LED_BRIGHT))
        {
            logger("Waiting for event or timeout...");
            WaitForSingleObject(hEvent, flashlightTimeout * 1000);

            logger("Disabling flashlight...");
            cam->SetFlashMode(ICamDevice::LED_OFF);
        }
        else
        {
            logger("This device does not support the bright mode...");
            DisplayError(L"This device does not support the bright mode.");
        }

    }
    else if (bBlink)
    {
        // Blink mode (non-bright)
        logger("Running in blink mode...");

        DWORD dwRunning = 0;
        while (dwRunning < flashlightTimeout * 1000)
        {
            // Enable flashlight
            cam->SetFlashMode(ICamDevice::LED_NORMAL);

            // Sleep for configured time
            Sleep(blinkTimeOnInMs);

            // Check if event has been triggered
            if (IsEventSet(hEvent))
                break;

            // Disable flashlight
            cam->SetFlashMode(ICamDevice::LED_OFF);

            // Sleep for configured time
            Sleep(blinkTimeOffInMs);

            // Check if event has been triggered
            if (IsEventSet(hEvent))
                break;

            // Increase the running time
            dwRunning += (blinkTimeOffInMs + blinkTimeOnInMs);
        }

        // Reset flashlight in case it was still on
        cam->SetFlashMode(ICamDevice::LED_OFF);

        logger("Done with the flashlight in blink mode...");
    }
    else if (bSos)
    {
        // SOS signal
        logger("Running in SOS mode...");

        // Convert SOS code to normal char
        int len = wcslen(sosText);
        char* text = new char[len+1];
        wcstombs(text, sosText, len);

        // The char array was +1 intentionally, as we need to close the string with a \0
        text[len] = '\0';

        // Calculate times (thanks to NLS @ xda-devs for letting me know - also seems to be on wikipedia ;))
        int dotTime = sosUnitTimeInMs; // short mark, dot or 'dit' (·) — one unit long
        int dashTime = sosUnitTimeInMs * 3; // longer mark, dash or 'dah' (–) — three units long
        int intraCharGapTime = sosUnitTimeInMs; // intra-character gap (between the dots and dashes within a character) — one unit long
        int shortGapTime = sosUnitTimeInMs * 3; // short gap (between letters) — three units long
        int mediumGapTime = sosUnitTimeInMs * 7; // medium gap (between words) — seven units long

        DWORD dwRunning = 0;
        bool bDone = false;
        bool bPreviousCharWasSpace = true;

        // Keep going until we reach a timeout or we're done due to the event being triggered
        while (dwRunning < flashlightTimeout * 1000 && !bDone)
        {
            // Loop through all letters in our string
            for (int i=0; text[i] != '\0'; i++)
            {
                // Check if event to stop has been set
                if (IsEventSet(hEvent))
                {
                    // Set flag to notify while loop to bail out
                    bDone = true;
                    break;
                }

                // If this is a space we just need to sleep
                if (text[i] == ' ')
                {
                    // This is the time between words
                    Sleep(mediumGapTime); dwRunning += mediumGapTime;
                    bPreviousCharWasSpace = true;
                    continue;
                }

                // If the previous character was not a space, we need a delay between characters
                if (!bPreviousCharWasSpace)
                {
                    Sleep(shortGapTime); dwRunning += shortGapTime;
                }

                // Reset this value as this character is not a space - next time we need a short gap
                bPreviousCharWasSpace = false; 

                // Convert this letter to morse code
                char *morse = LetterToMorse(text[i]);

                // Loop through the morse code
                bool bSkipIntraGap = true;
                for (int j=0; morse[j] != '\0'; j++)
                {
                    // Check if event to stop has been set
                    if (IsEventSet(hEvent))
                    {
                        // Set flag to notify while loop to bail out
                        bDone = true;
                        break;
                    }

                    // Sleep a moment between dots and dashes
                    if (bSkipIntraGap)
                    {
                        bSkipIntraGap = false;
                    }
                    else
                    {
                        Sleep(intraCharGapTime); dwRunning += intraCharGapTime;
                    }

                    // Short signal?
                    if (morse[j] == '.')
                    {
                        cam->SetFlashMode(ICamDevice::LED_NORMAL);
                        Sleep(dotTime); dwRunning += dotTime;
                        cam->SetFlashMode(ICamDevice::LED_OFF);
                    }

                    // Long signal?
                    if (morse[j] == '_')
                    {
                        cam->SetFlashMode(ICamDevice::LED_NORMAL);
                        Sleep(dashTime); dwRunning += dashTime;
                        cam->SetFlashMode(ICamDevice::LED_OFF);
                    }
                }
            }

        }

        logger("Disabling SOS mode...");
    }
    else if (bPtt)
    {
        // Push-to-talk mode (non-bright, enabled as long as a key is pressed)
        logger("Running in Push-To-Talk mode...");

        // Determine the key that is currently pressed
        int currentlyPressedKey = 0;
        for (int i=0x01; i <= 0xFE; i++) // VK_ range
        {
            // We only check the most significant bit, as it reflects the _actual_ state of the key
            if (GetAsyncKeyState(i) & 0x8000)
            {
                logger("Monitoring key '%d' as it is currently pressed...", i);
                currentlyPressedKey = i;
                break;
            }
        }
        if (currentlyPressedKey)
        {
            // Enable the flashlight
            logger("Enabling flashlight...");
            cam->SetFlashMode(ICamDevice::LED_NORMAL);

            // Wait for the key to be released
            logger("Waiting for key '%d' to be released...", currentlyPressedKey);
            while (GetAsyncKeyState(currentlyPressedKey) & 0x8000)
                Sleep(100);

            // Key released, turn the light off
            logger("The key was released, disabling the flashlight...");
            cam->SetFlashMode(ICamDevice::LED_OFF);
        }
        else
        {
            logger("There are no keys currently pressed. Either it was released already or we cannot detect the keypress.");
        }
    }
    else
    {
        // Normal mode
        logger("Running in normal mode...");

        logger("Enabling flashlight...");
        cam->SetFlashMode(ICamDevice::LED_NORMAL);

        logger("Waiting for event or timeout...");
        WaitForSingleObject(hEvent, flashlightTimeout * 1000);

        logger("Disabling flashlight...");
        cam->SetFlashMode(ICamDevice::LED_OFF);
    }

    logger("Closing down...");

    // Deinitialize the cam
    cam->Deinitialize();

    // Close the event
    CloseHandle(hEvent);

    return 0;
}

char* LetterToMorse (char c)
{
    switch (c)
    {
        case 'a':
        case 'A': 
            return "._";
        case 'b':
        case 'B': 
            return "_...";
        case 'c':
        case 'C':
            return "_._.";
        case 'd':
        case 'D':
            return "_..";
        case 'e':
        case 'E':
            return ".";
        case 'f':
        case 'F':
            return ".._.";
        case 'g':
        case 'G':
            return "__.";
        case 'h':
        case 'H':
            return "....";
        case 'i':
        case 'I':
            return "..";
        case 'j':
        case 'J':
            return ".___";
        case 'k':
        case 'K':
            return "_._";
        case 'l':
        case 'L':
            return "._..";
        case 'm':
        case 'M':
            return "__";
        case 'n':
        case 'N':
            return "_.";
        case 'o':
        case 'O':
            return "___";
        case 'p':
        case 'P':
            return ".__.";
        case 'q':
        case 'Q':
            return "__._";
        case 'r':
        case 'R':
            return "._.";
        case 's':
        case 'S':
            return "...";
        case 't':
        case 'T':
            return "_";
        case 'u':
        case 'U':
            return ".._";
        case 'v':
        case 'V':
            return "..._";
        case 'w':
        case 'W':
            return ".__";
        case 'x':
        case 'X':
            return "_.._";
        case 'y':
        case 'Y':
            return "_.__";
        case 'Z':
        case 'z':
            return "__..";
        case ',':
            return "__..__";
        case '.':
            return "._._._";
        case '1':
            return ".____";
        case '2':
            return "..___";
        case '3':
            return "...__";
        case '4':
            return ".____";
        case '5':
            return ".....";
        case '6':
            return "_....";
        case '7':
            return "__...";
        case '8':
            return "___..";
        case '9':
            return "____.";
        case '0':
            return "_____";
        default:
            return "";
    }
}