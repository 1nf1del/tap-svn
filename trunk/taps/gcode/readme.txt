GCode TAP distribution

author: 	John De Angelis
date:		19 october 2004
version:	v1.6

What's different in this version?:

	- fixed bugs in timer details edit screen
	- added tasmanian channels file

About:

The Gcode tap allows you to create timer entries by using GCodes found in printed TV guides. The GCode algorithm is the same as Europe's Showview so this tap should work Showview as well.

Limitation: This TAP only supports GCodes of 1-6 characters in length (by far the most common). The algorithm for decoding longer codes has not been made public yet...


Installation:

If you get non-Sydney channels for 2,7,9,10 you *must* change the gcode.ini file and enter the correct channel names and numbers corresponding to your location. Some location's ini files have been included in the distribution - if they have simply rename your location's file to gcode.ini.

To have this tap run every time you start your Toppy, copy the files gcode.tap, gcode.ini, and keymap.ini to the Toppy's Auto Start directory using Altair. If you want the program to manually start, copy to any other directory and run the program from the file list screen.

You can alter the keys that the tap uses by altering the keymap.ini file.


The Kit:

The distribution contains full sources to rebuild the application. In order to successfully build the kit you will need to:

- install the cygwin compiler
- install Topfield's TAP environment
- copy Topfield's TAP files (libtap.a, etc) to c:\work\tap.
- from the command line, run nmake


License:

This software may be used for personal use only. Any use of this software for commercial purposes is prohibited unless a specific agreement has been made with the author for that purpose.

Contact:

email me at j_de_angelis@hotmail.com. Please understand that I have a normal job and a family and that I don't read email every day...

Sources:

www.zelczak.com - great website describing the Showview coding algorithm - sample Javascript for decoding
Daniel Minder - [daniel@minder.de] - describes Showview algorithm
Other TAP writers including tonymy01 (http://tonyspage.abock.de/), andy cullen (http://members.westnet.com.au/web/acullen/topfield.htm) and others.

Testing:

Big thanks to robo for helping me wring out the bugs....

