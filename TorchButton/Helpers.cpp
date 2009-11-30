//
// Helper functions implementations
//

#include "stdafx.h"

#include "Helpers.h"

extern const wchar_t* REGKEY_BASE;
extern const wchar_t* REGVALUE_VALUE_DEBUG_ENABLED;
extern const DWORD REGVALUE_DEFAULT_DEBUG_ENABLED = 0;

char *append_char ( const char *s, const char c , BOOL appendNull)
{
  size_t len = strlen ( s );

  char *ret;
  if (appendNull)
      ret = new char[len + 2];
  else
      ret = new char[len + 1];

  strcpy ( ret, s );
  ret[len] = c;

  if (appendNull)
    ret[len + 1] = '\0';

  return ret;
}

static int logger_debug_enabled = -1;
void logger(const char *format, ...)
{
    // Check if logging is enabled in the registry and cache the value
    if (logger_debug_enabled == -1)
    {
        logger_debug_enabled = 0; // prevent re-entry from ReadValue(..);
        HKEY hKey = 0;
        if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGKEY_BASE, NULL, NULL, REG_OPTION_NON_VOLATILE, NULL, NULL, &hKey, NULL) == ERROR_SUCCESS)
        {
            DWORD dwSize;
            dwSize = sizeof(DWORD);
            ReadValue(hKey, REGVALUE_VALUE_DEBUG_ENABLED, REG_DWORD, (BYTE*)&logger_debug_enabled, &dwSize, (BYTE*)&REGVALUE_DEFAULT_DEBUG_ENABLED, sizeof(DWORD));
            RegCloseKey(hKey);
        }
        else
        {
            logger_debug_enabled = 0;
        }
    }

    // Only log when enabled
    if (logger_debug_enabled > 0)
    {
        // Open file
        FILE *handle;
        handle = fopen("\\torchbutton.txt", "ab");

        // Append newline
        char* newformat = append_char(format, '\n', true);

        // Print to console
        va_list ap;
        va_start (ap, format);
        vprintf(newformat, ap);
        va_end (ap);

        // Call vfprintf to write with formatting
        va_start (ap, format);
        vfprintf(handle, newformat, ap);
        va_end (ap);

        // Close up
        fclose(handle);
    }
}

BOOL IsEventSet(HANDLE hEvent)
{
    DWORD dwEventResult = WaitForSingleObject(hEvent, 0);
    if (dwEventResult == WAIT_OBJECT_0 ||
        dwEventResult == WAIT_ABANDONED)
    {
        logger("The event has been set.");
        return true;
    }
    return false;
}

void DisplayError(wchar_t *error)
{
    MessageBox(NULL, error, L"Error", MB_OK);
}

BOOL ReadValue(HKEY hKey, LPCWSTR valueToRead, DWORD dwType, LPBYTE lpData, LPDWORD lpcbSize, BYTE* lpDefaultValue, DWORD lpcbDefaultSize)
{
    // Read the value into blinkTimeOffInMs
    logger("Reading the value of %s...", valueToRead);
    if (RegQueryValueEx(hKey, valueToRead, NULL, NULL, lpData, lpcbSize) != ERROR_SUCCESS)
    {
        // An error occurred when reading the date, it probably doesn't exist. Set it with the default.
        if (RegSetValueEx(hKey, valueToRead, NULL, dwType, lpDefaultValue, lpcbDefaultSize) != ERROR_SUCCESS)
        {
            // An error occurred when setting the default data
            logger("Error while setting the default value for '%s', aborting...", valueToRead);
            DisplayError(L"Error while setting a default registry value, aborting...");
            return false;
        }

        // Now try to read it again
        if (RegQueryValueEx(hKey, valueToRead, NULL, NULL, lpData, lpcbSize) != ERROR_SUCCESS)
        {
            logger("Error while retrieving the registry value for '%s', even after setting default...", valueToRead);
            DisplayError(L"Error while retrieving a registry value, after setting the default. Aborting!");
            return false;
        }
    }
    return true;
}
