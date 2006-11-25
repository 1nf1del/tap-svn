MakeLogos 1.0
----------------

MakeLogos is a command line replacement for the MakeLogos TAP. It can process gif, png, jpeg and pcx files and will output a logo.dat or smlogo.dat file for use with UKTimers, Archive, Surfer, AutoSearch and MyStuff 5.

Examples
--------
Process all png files
makelogos *.png

Process all gif and pcx files with a name beginning UI and output backup_logo.dat
makelogos -g -obackup_logo.dat *.gif UI*.pcx

Source Code
-----------
Source can be found at http://tap.berlios.de/

Acknowledgements
----------------
MakeLogos uses the FreeImage library for graphics loading which can be found at http://freeimage.sourceforge.net/

Release History
---------------
1.0
Initial release

Simon Capewell
simon.capewell@gmail.com
