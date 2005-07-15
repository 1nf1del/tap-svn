libUtils distribution

author: 	John De Angelis
date:		15 May 2005
version:	v1.2

About:

libUtils is a package of functions/utilities of interest to programmers of Topfield's TF5000PVRt TAPs. It contains the following:

- a basic implementation of the System V file functions, fopen/fclose/fgets/fputs (borrowed extensively from K&R)
- a basic implementation of Win32 INI GetPrivateProfileString/WritePrivateProfileString
- a key mapper utility that externalises a TAPs Remote Control key assignment. This would allow a end user to modify the key assignments for the TAP to his/her liking.
- various utility functions such as vsprintf and strtok implementations otherwise not available on the Topfield

The Kit:

The distribution contains full sources to rebuild the application. Documentation for the library is in the "Doco" folder. In order to successfully build the kit you will need to:

- install the cygwin compiler
- install Topfield's TAP environment
- copy Topfield's TAP files (libtap.a, etc) to c:\work\tap.
- copy this kit's tap.ld to c:\work\tap
- from the command line, run nmake in the directory where you installed libUtils

Also I have included some sample TAPs that use the functions in the library. These are in the samples folder.


License:

This software may be used for personal use only. Any use of this software for commercial purposes is prohibited unless a specific agreement has been made with the author for that purpose.

Contact:

email me at j_de_angelis@hotmail.com. Please understand that I have a normal job and a family and that I don't read email every day...
