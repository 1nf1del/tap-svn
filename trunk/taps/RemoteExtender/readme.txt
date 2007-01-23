Remote Extender 1.4
-------------------

Remote Extender is a TAP that, whilst running, patches the firmware to allow TAPs to interpret extra remote codes from programmable remotes. On the TF5800 it also allows TAPs to distinguish between TV/Radio and List, and the arrow keys and volume and channel keys. The most common use of Remote Extender is to allow the MyStuff Info Box to behave more intuitively. Just put the standard or MHEG version into AutoStart if you're running MyStuff.

At any time, you can shut down Remote Extender by pressing the front panel OK button 5 times in succession. This is a good way to revert to the standard remote if you're running the Toppy2 version of Remote Extender.

Remote Extender has no options screen and instead comes in 4 versions: Standard, MHEG and Toppy2 and Toppy2MHEG.

MHEG
----
MHEG versions are for the TF5800 only and fix a bug in the firmware that prevents TAPs from detecting Teletext. This is particularly useful for Jag's EPG as it will allow OK to be used to activate the Channel List view without blocking entry of Teletext page numbers.

Toppy2
------
Toppy2 will be of interest to people with 2 Topfield PVRs. It translates codes from a programmable remote into standard Topfield commands to allow you to operate each independently. All buttons except Power are supported. The codes to base your remote's programme on are listed below.

5800/5000 key	Normal Remote	Toppy2 Second Remote
Up				0x00			0x20 
Down			0x01			0x21 
Right			0x02			0x22 
Left			0x03			0x23 
List/TVRad		0x04			0x24 
Exit/AudioTrk	0x05			0x25
Pause			0x06			0x26 
ChUp/Subt		0x07			0x27
TvSat			0x08			0x28 
VolUp/Fav		0x09			0x29
Power			0x0a			0x2a	Note that Power cannot be handled by Remote Extender, the code is listed here for completeness
Archive			0x0b			0x2b	Alternate to PlayList		
Mute	 		0x0c			0x2c
Green			0x0d			0x2d 
Yellow			0x0e			0x2e 
Blue	 		0x0f			0x2f 
0	 			0x10			0x30 
1	 			0x11			0x31 
2	 			0x12			0x32 
3	 			0x13			0x33 
4	 			0x14			0x34 
5	 			0x15			0x35 
6	 			0x16			0x36 
7	 			0x17			0x37 
8	 			0x18			0x38 
9	 			0x19			0x39 
Menu	 		0x1a			0x3a 
Guide	 		0x1b			0x3b 
VolDown/Exit	0x1c			0x3c
Info	 		0x1d			0x3d 
Recall			0x1e			0x3e 
Ok	 			0x1f			0x3f 
TVRadio/Uhf		0x43			0x53 
Opt/Sleep		0x44			0x54 
Rewind			0x45			0x55 
Play	 		0x46			0x56 
ChDown/Teletext	0x47			0x57 
Forward	 		0x48			0x58 
Slow	 		0x49			0x59 
Stop	 		0x4a			0x5a 
Record	 		0x4b			0x5b 
White/Ab		0x4c			0x5c 
Red	 			0x4d			0x5d 
PIP/Prev		0x50			0x60 
PlayList		0x51			0x61
Teletext/Next	0x52			0x62 
PIPSwap/Sat	 	0x5e			0x6e

Topfield have a special service remote that displays test patterns and corrupts the hard disk at the touch of a button. Avoid these codes unless you are very brave!
Cmd_0			0x70	Test colour bars
Cmd_1			0x71	Blank screen
Cmd_2			0x72	?
Cmd_3			0x73	All channels go blank
Cmd_4			0x74	All channels go blank
Cmd_5			0x75	?
Cmd_6			0x76	Jump to the start of timeshift/record buffer
Cmd_7			0x77	?
Cmd_8			0x78	?
Cmd_9			0x79	?
Cmd_a			0x7a	CORRUPT HARD DISK - Overwrites the first 1024 bytes of the hard disk with 0xff
Cmd_b			0x7b	Test bars – black and white split half screen
Cmd_c			0x7c	Test bars – black and red split half screen
Cmd_d			0x7d	Test bars – black and green split half screen
Cmd_e			0x7e	Test bars – black and blue split half screen
Cmd_f			0x7f	FACTORY RESET


Information for TAP Developers
------------------------------
Remote Extender patches the firmware to make the EVT_KEY event send the internal remote keycodes in param2. RawKey.h contains definitions of these codes. If the value of param2 is 0 then Remote Extender is either not running, or does not have an internal keycode available (usually due to the keypress being caused by TAP/GenerateEvent) and param1 should be processed instead.

Suggested uses for this are:
On the TF5800, for distinguishing between V+,V-, P+,P- and the arrow buttons and TV/Radio and List.
On all models, for providing extra functions for users of programmable remotes.

History
-------
1.4		Support for 5800 21/12/2006 (13.39) firmware.
1.3		Support for 5800 17/08/2006 (12.99) firmware.
1.2		Support for 5800 19/09/2006 (13.26) firmware.
1.1		Support for 5800 14/07/2006 (12.88) firmware.
1.0		Initial release

Simon Capewell
simon.capewell@gmail.com
