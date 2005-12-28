*TFDPACK*


Files:

AR.H
TFDPACK.C
IO.C
MAKETBL.C
MAKETREE.C
HUF.C
ENCODE.C
DECODE.C

Executable: TFDPACK.EXE

This programme was written as a beginner's exercise in 'C' programming.
It has been compiled with an old Borland C++ 4.5 compiler as  DOS using a Compact Model.

The type definitions  are based on the original AR002 version and
compiling with a Win32 model gives lots of warnings about conversion and loss of digits.
This is probably not critical but I don't yet fancy the challenge of restructuring the 
Huffman sections of the code.

The programme is based on AR002 by Haruhiko Okumura. Only the main file has been rewritten and
commented to provide a suitable link to the encode and decode routines.

Firebird provided the detailed analysis of the file structure for the TF5### series of
PVRs which helped considerably.

Please free to rewrite, modify, recompile or distribute this programme.


email: dww @ berkswich.co.uk
or personal message via dwright on www.topp.org.uk.


The block type is hard coded as 0001. This is correct for the firmware.
However Firebird has shown several other block type being used for settings
and loader files. These block types use a much smaller block size.
The maximum block size of 32762 is probably limited by addressing space in a buffer
for the CRC calculations. 

Eventually I may write a Windows graphical front end or convert this programme to a 
'C' dll for use with Visual Basic.

David Wright

Milford,Stafford,UK  03/12/2005

TFDPACK modified 12/12/2005 to stop it deleting the extracted file!! See 2nd last line 