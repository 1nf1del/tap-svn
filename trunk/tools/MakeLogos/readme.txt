MakeLogos 1.2
-------------
MakeLogos is a command line replacement for the MakeLogos TAP. It can process gif, png, jpeg and pcx files and will output a logo.dat or smlogo.dat file for use with UKTimers, Archive, Surfer, AutoSearch and MyStuff 5. It can also run in extract mode to create png files from a logo file, handy if you don't have source graphics available.

Examples
--------
Process all png files
makelogos *.png

Process all gif and pcx files with a name beginning UI and output backup_logo.dat
makelogos -obackup_logo.dat *.gif UI*.pcx

Extract graphics from logo.dat and put them in the source_graphics directory
makelogos logo.dat -x -dsource_graphics


Source Code
-----------
Source can be found at http://tap.berlios.de/

Acknowledgements
----------------
The logo utilities use the FreeImage library for graphics loading which can be found at http://freeimage.sourceforge.net/

Release History
---------------
1.2		Extract option added
1.1		Better support for transparency
1.0		Initial release

Simon Capewell
simon.capewell@gmail.com
