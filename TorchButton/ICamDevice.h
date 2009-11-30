//
// ICamDevice.h
//

#pragma once

#include "stdafx.h"

DeclareInterface(ICamDevice)
    // Defines the operational modes for the flash
    enum FlashModes { LED_OFF, LED_NORMAL, LED_BRIGHT };

    // Does initialization stuff, such as loading libraries, memory mapping, init function calls, etc.
    // Returns true when succesful, or false when not.
    bool Initialize();

    // Deinitializes stuff
    void Deinitialize();

    // Sets a specific flash mode
    // Returns true when succesful, or false when not (i.e. not implemented)
    bool SetFlashMode(FlashModes mode);
EndInterface(ICamDevice)
