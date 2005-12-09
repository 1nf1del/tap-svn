MakeGraphics 1.0
----------------

Makegraphics is a command line replacement for Topfield's mkgd GD Generator program. You can use it as part of your build process as a rule in your makefile to automatically process any graphics files.

MakeGraphics can process gif, png, jpeg and pcx files and will output in either compressed gd format or pixmap format.

Examples
--------
Process all png files and output compressed gd data to png_graphics.c and png_graphics.h
makegraphics -g -opng_graphics.c *.png

Process all gif and pcx files and output pixmap data to png_graphics.c and png_graphics.h
makegraphics -g -opng_graphics.c *.gif *.pcx

Process UI*.pcx files into individual files in uncompressed pixmap format
makegraphics -p UI*.pcx

Source Code
-----------
Source can be found at http://tap.berlios.de/

Acknowledgements
----------------
Thanks to Firebird for deciphering the gd format
MakeGraphics uses the FreeImage library for graphics loading which can be found at http://freeimage.sourceforge.net/

Simon Capewell
simon.capewell@gmail.com
