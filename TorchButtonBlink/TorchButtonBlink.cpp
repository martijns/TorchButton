#include <windows.h>
#include <iostream>
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
    // Retrieve the full path to current executable
    _TCHAR exefilename[_MAX_DIR];
    if (GetModuleFileName(NULL, exefilename, _MAX_DIR) == NULL)
    {
        MessageBox(NULL, L"Error fetching the full path to the executable!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Convert to string for easy handling
    std::wstring exefile(exefilename);

    // Replace exe
    std::wstring find(L"TorchButtonBlink.exe");
    std::wstring replacement(L"TorchButton.exe");
    size_t found;
    found = exefile.rfind(find);
    if (found != std::string::npos)
    {
        exefile.replace(found, find.length(), replacement);
    }
    else
    {
        MessageBox(NULL, L"Error finding TorchButtonBlink.exe in current filename!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Create string with switch
    std::wstring exefilearg(exefile);
    exefilearg.append(L" /blink");
    
    // Start process
    BOOL bWorked;
    STARTUPINFO suInfo;
    PROCESS_INFORMATION procInfo;

    memset(&suInfo, 0, sizeof(suInfo));
    suInfo.cb = sizeof(suInfo);

    bWorked = ::CreateProcess(
        exefile.c_str(),
        exefilearg.c_str(),
        NULL,
        NULL,
        FALSE,
        NULL,
        NULL,
        NULL,
        &suInfo,
        &procInfo);

    // Notify user if it failed
    if (!bWorked)
    {
        MessageBox(NULL, L"Error starting the TorchButton.exe process with /blink parameter!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // All went well
    return 0;
}
