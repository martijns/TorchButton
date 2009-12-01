.----------------------------------------------------------------------
| TorchButton, v2.2
}----------------------------------------------------------
|
| Dropped your keys? Finding something behind the bench? Operating on a computer and
| need to find that jumper on the mainboard?
|
| This is a simple application built for some HTC devices that have a hardware LED,
| normally used to take pictures in the dark. However I found the LED to be perfect
| to be used as a handy pocket flashlight.
|
| Usage:
|   When the program starts, the flashlight is turned on. It will keep the
|   light on for 60 seconds (configurable in registry). You can turn the light
|   off earlier by starting the program again. This applies to all modes of the
|   application.
|
| Modes:
|   TorchButton has 5 modes so far. Each one explained in short:
|   
|   Normal:   Simply enable the flashlight until the application is started again or
|             the configurable timeout occurs (60 seconds by default). This mode
|             can be used for a prolonged period. I have only tested it up to 5 minutes
|             though.
|             
|   Bright:   The Bright mode is exactly the same as the Normal mode, with the only
|             difference being that the LED is more bright. This mode is equal to the
|             short moment when you make a photo with flashlight on. Note that this mode
|             does stress the LED and should not be used for prolonged periods. On the
|             Touch Pro I have used this up to a minute without problems. This feature
|             may or may not be available on some devices.
|
|   Blink:    The Blink mode turns the LED on and off in specific intervals that you
|             can configure in the registry. An example of usage is the bike light.
|             
|   SOS:      This is extensive mode that supports sending custom morse code. The text
|             to be sent via morse code signals can be configured in the registry. This
|             defaults to "sos ", thus the SOS name for this mode. NOTE that the flashlight
|             timeout does NOT interrupt a text. It checks if the timeout occurred when it
|             starts over again and quits when it reached the timeout.
|
|   PTT:      The Push-To-Talk/Torch mode. When you device has a key you can map to 'hold',
|             you can use this feature to keep the light on as long as the key is pressed.
|             The AT&T Fuze is one of those devices with a PTT button.
|
| Configuration settings:
|   All settings can be configured via the Registry. All values can be found under
|   the HKLM\Software\TorchButton\ key. An up-to-date list of configuration settings is
|   presented below. Note that they are case-sensitive.
|
|   FlashlightTimeout (DWORD), default 60. In seconds.
|      Configures the timeout after which the application should automatically stop. This
|      timeout applies to all variants.
|   
|   blinkTimeOnInMs (DWORD), default 250. In milliseconds.
|      When using blink mode, determines how long the led should stay on.
|   
|   blinkTimeOffInMs (DWORD), default 250. In milliseconds.
|      When using blink mode, determines how long the led should stay off before turning
|      on again.
|
|   BlinkUsesBrightMode (DWORD), default 0.
|      When set to 1, blink mode will make use of the bright mode. If bright mode is not
|      available for a certain device, it will fall back to normal mode.
|   
|   sosText (SZ), default "sos ". Textual.
|      When using SOS mode, determines the text to send with morse code. Supported
|      characters are a-z, 0-9, comma (,) and period (.). Any non-recognized characters
|      are simply ignored. A space character inserts a pause of 7 units. This text may be
|      up to 8KB.
|      
|   sosUnitTimeInMs (DWORD), default 100. In milliseconds.
|      When using SOS mode, this determines the unit time that serves as base for the
|      various timing. The unit time is the same as how long a "dot" should last.
|
|   brightResetTimeInMs (DWORD), default 750. In milliseconds.
|      The bright mode uses a mechanism to keep the LED on the mode that is normally only
|      used when making a picture with flashlight. A safety in the driver prevents this
|      mode from being active too long. I've worked around it by quickly re-enabling the
|      bright LED. On some devices this reset time should be shorter.
|      NOTE: Set this to 100 when you have the Alltel Touch Pro.
|
|   BrightModeTimeout (DWORD), default 10. In seconds.
|      A seperate timeout used for bright mode. As bright mode can cause the led(s) to heat
|      up a lot more than normal mode, it is limited by a seperate timeout.
|
|   DebugEnabled (DWORD), default 0.
|      When set to 1, a log file will be created in the root directory called
|      torchbutton.txt. Include this file if there are issues.
|
| History:
|
|   v2.2 (2009-12-02)
|   - New icons for all TorchButton apps (thanks tnyynt!). These only work in WM6.5 and up.
|     Older OS's will still see the old icons.
|   - Added registry setting to use bright setting in blink mode (see BlinkUsesBrightMode).
|   - Bright mode now has a seperate timeout, configurable in registry (see BrightModeTimeout).
|   - Added support for Samsung Omnia i9x0 devices (hopefully they all work).
|     Thanks to raph/zemrwhite2/PaSSoA (http://www.modaco.com/content-page/275325/camera-light-and-software/page/20/#entry936759).
|     Thanks to Chainfire for testing on his Omnia i900L.
|
|   v2.1 (2009-11-14)
|   - Bright mode is now support on the HTC HD2 (Leo).
|   - Added a one-time warning message when bright mode is used for the first time.
|
|   v2.0 (2009-11-07)
|   - Added support for the HTC HD2 (Leo). Bright mode does not work on this device. Maybe later...
|
|   v1.5 (2009-04-28)
|   - Added PTT (Push-To-Torch) mode. Probably only useful for devices that have the PTT button,
|     such as the AT&T Fuze.
|   - Added brightResetTimeInMs registry setting for owners of the Alltel Touch Pro. They need to
|     set this value to 100 for bright mode to work properly.
|
|   v1.4 (2008-11-29)
|   - Fixed 'sos' mode. It now properly uses a single configurable unit time to calculate
|     other intervals.
|   - Updated 'sos' mode. You can now configure a text to be sent via morse code in the
|     registry. Check configuration part of the readme for more info.
|   - Removed old 'sos' registry settings and replaced them with new ones.
|   - Increased default flashlight timeout from 30 to 60 seconds.
|   - Changed default 'blink' time to 250ms for both on and off.
|
|   v1.3 (2008-11-13)
|   - Added 'blink' feature. Registry configuration options:
|     - blinkTimeOnInMs (DWORD), default 500. Configures the time the LED is on.
|     - blinkTimeOffInMs (DWORD), default 500. Configures the time the LED is off.
|   - Added 'SOS' feature. More like a 'morse code' feature though. You can set any
|     morse code to be signalled in the registry. Options:
|     - sosCode (SZ), default " ...---...". Configures the morse code to signal.
|     - sosDotTimeoutInMs (DWORD), default 200. Configures 200ms LED on, and 200ms LED off for ".".
|     - sosDashTimeoutInMs (DWORD), default 400. Configures 400ms LED on, and 400ms LED off for "-".
|     - sosSpaceTimeoutInMs (DWORD), default 1000. Configures 1000ms pause when processing space " ".
|   - New shortcuts are added for those features.
|
|   v1.2 (2008-11-09)
|   - Enlarged maximum timeout override from 300 seconds to 86400 seconds (a day).
|   - Added 'bright' feature. Note that the light may flicker almost unnoticably
|     every 750ms, can't help that. :)
|
|   v1.1 (2008-09-01)
|   - Added ability to override default timeout value (in seconds) stored in
|     HKLM\Software\TorchButton\FlashlightTimeout (DWORD).
|
|   v1.0 (2008-08-31)
|   - Initial version
|
}-------------------------------
|
| Author:     Martijn Stolk
| Website:    www.netripper.nl
| XDA thread: http://forum.xda-developers.com/showthread.php?p=4943210
|
`------------------
