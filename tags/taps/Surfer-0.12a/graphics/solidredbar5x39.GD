#include "type.h"
byte _solidredbar5x39Cpm[] = 
{
0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0x19,0x93,0xb0,0xff,0xff,
0x01,0x86,0x00,0x07,0x30,0x69,0x67,0xfd,0xe7,0x3b,0x0f,0x46,0x04,0x71,0x2a,0x40,
0x10,0x97,0x6e,0x38,0x40,
};


TYPE_GrData _solidredbar5x39Gd =
{
	1,		//version
	0,		//reserved
	OSD_1555,		//data format
	COMPRESS_Tfp,		//compressed method
	_solidredbar5x39Cpm,		//data
	390,		//size
	5,		//width
	39		//height
};
