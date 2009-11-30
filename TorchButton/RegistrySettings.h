#pragma once

// Some constants
const wchar_t* EVENT_NAME = L"GLOBAL\\{3BC822CC-9330-4b94-983A-281418FA54AC}";
const wchar_t* REGKEY_BASE = L"Software\\TorchButton";

const wchar_t* REGVALUE_VALUE_FLASHLIGHT_TIMEOUT = L"FlashlightTimeout";
const wchar_t* REGVALUE_VALUE_BLINK_TIME_ON_IN_MS = L"blinkTimeOnInMs";
const wchar_t* REGVALUE_VALUE_BLINK_TIME_OFF_IN_MS = L"blinkTimeOffInMs";
const wchar_t* REGVALUE_VALUE_SOS_TEXT = L"sosText";
const wchar_t* REGVALUE_VALUE_SOS_UNIT_TIME_IN_MS = L"sosUnitTimeInMs";
const wchar_t* REGVALUE_VALUE_BRIGHT_RESET_TIME_IN_MS = L"brightResetTimeInMs";
const wchar_t* REGVALUE_VALUE_DEBUG_ENABLED = L"DebugEnabled"; // Used in Helpers.cpp
const wchar_t* REGVALUE_VALUE_BRIGHT_WARNING_ACCEPTED = L"BrightWarningAccepted";

const DWORD REGVALUE_DEFAULT_FLASHLIGHT_TIMEOUT = 60; // Seconds
const DWORD REGVALUE_DEFAULT_BLINK_TIME_ON_IN_MS = 250; // Milliseconds
const DWORD REGVALUE_DEFAULT_BLINK_TIME_OFF_IN_MS = 250; // Milliseconds
const wchar_t* REGVALUE_DEFAULT_SOS_TEXT = L"sos ";
const DWORD REGVALUE_DEFAULT_SOS_UNIT_TIME_IN_MS = 100;
const DWORD REGVALUE_DEFAULT_BRIGHT_RESET_TIME_IN_MS = 750;
const DWORD REGVALUE_DEFAULT_DEBUG_ENABLED = 0; // Used in helpers.cpp
const DWORD REGVALUE_DEFAULT_BRIGHT_WARNING_ACCEPTED = 0;
