#pragma once

// Helper functions prototypes
char* LetterToMorse (char c);
char* append_char ( const char *s, const char c , BOOL appendNull);
void logger(const char *format, ...);
BOOL IsEventSet(HANDLE hEvent);
void DisplayError(wchar_t *error);
BOOL ReadValue(HKEY hKey, LPCWSTR valueToRead, DWORD dwType, LPBYTE lpData, LPDWORD lpcbSize, BYTE* lpDefaultValue, DWORD lpcbDefaultSize);
