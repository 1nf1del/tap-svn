Virtual Remote for Topfield PVRs

This TAP and corresponding PC program emulates the Topfield remote via the serial cable.
The PC client contains a debugger output window and a TV window if you have a TV tuner installed in your PC.

The TAP uses the built in debugger command interface to send keypresses, which is much more reliable than TAP_GenerateEvent.

Limitations:
Currently this TAP only works 100% correctly on the TF5800, however with some changes to the key mappings and better scanning of the firmware it should work on other models.
The firmware is patched to prevent incoming serial information being intercepted by the firmware. This drastically increases the reliability of the keypresses.

Simon Capewell
simon.capewell@gmail.com

