#include "type.h"
byte _greenbar10x39Cpm[] = 
{
0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0x72,0xd1,0xbf,0xff,0xff,
0x03,0x0c,0x00,0x4f,0x54,0x53,0x89,0x4a,0x76,0x62,0xe8,0x29,0xd0,0x5f,0xba,0x0a,
0x9d,0x05,0x7c,0x72,0x02,0x04,0x03,0x59,0xfe,0x04,0x04,0x04,0x04,0x02,0x04,0xe6,
0x2e,0x82,0xa7,0x41,0x0e,0x4b,0xf2,0x1c,0xd4,0x2c,0x27,0x51,0xd6,0x80,0x7d,0x41,
0x09,0x2c,0x80,0x6b,0xd0,0xb5,0xca,0xc5,0xe3,0x1c,0xa6,0x57,0x66,0x41,0x83,0x14,
0xc9,0x6f,0x65,0x60,0xbf,0xd8,0x2e,0xa3,0xa7,0xa9,0xad,0x7b,0xf5,0xe8,0x20,0x3e,
0x8d,0xe1,0x66,0x1c,0x67,0x82,0x5c,0x97,0x7c,0x72,0x62,0x96,0x75,0x36,0x6a,0xb7,
0x58,0x04,0x53,0xe0,0xe6,0x81,0x06,0x3f,0x22,0xe9,0x65,0x8e,0xcb,0xfc,
};


TYPE_GrData _greenbar10x39Gd =
{
	1,		//version
	0,		//reserved
	OSD_1555,		//data format
	COMPRESS_Tfp,		//compressed method
	_greenbar10x39Cpm,		//data
	780,		//size
	10,		//width
	39		//height
};
