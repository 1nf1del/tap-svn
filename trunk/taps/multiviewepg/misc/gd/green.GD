#include "type.h"
byte _greenCpm[] = 
{
0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0xd5,0x1b,0xdc,0xff,0xff,
0x02,0x00,0x00,0x9e,0x5a,0x57,0xd1,0x3d,0x21,0xbb,0xc0,0xad,0xde,0x05,0xde,0x05,
0x46,0xf0,0x26,0xe0,0xe3,0x1f,0x50,0x77,0xfc,0x12,0x39,0x48,0xe4,0xb2,0x14,0x44,
0x12,0x92,0x51,0x29,0x21,0x83,0x21,0x2b,0x29,0x90,0x94,0x91,0xc5,0xa3,0x1d,0xf5,
0xc4,0x5a,0xd9,0x08,0x91,0xd9,0x0c,0x8f,0xf5,0x24,0x11,0x36,0xdb,0x5f,0xf8,0xe9,
0x01,0x48,0x34,0x0a,0xcf,0x50,0x60,0x53,0xa4,0xc8,0xf4,0x19,0x2e,0x6b,0xbf,0xc4,
0x39,0x44,0xd3,0xb7,0xb1,0xa3,0xaf,0x62,0xc1,0x41,0x47,0xdc,0xf1,0xd5,0x2d,0x52,
0xc2,0x78,0x83,0x9f,0x4b,0x2b,0xa6,0x4e,0x3b,0xd6,0xc1,0x6f,0x26,0xed,0x95,0xd5,
0x2c,0x41,0x3c,0x76,0xdf,0x65,0x71,0x04,0x27,0xa6,0x4b,0xb0,0xd0,0xfe,0xbd,0x98,
0xd1,0x14,0x56,0x79,0x10,0x50,0x61,0x5f,0x78,0x9f,0x8f,0x05,0x7c,0x3f,0x18,0xfc,
0x1b,0x1f,0xb6,0xde,0x3d,0xbb,0x0b,0xb0,0x67,0x9e,0xfe,0x12,0xe0,0x29,0x94,0x75,
0x2d,0x4c,0xf8,0xc5,0xe9,0x72,0x67,0xf9,0x70,0x62,0xfc,0xf1,0xa5,0xa6,0x4f,0xeb,
0x60,0xd1,0xb4,0xfa,0x82,0xe7,0x57,0xc6,0x23,0xf7,0xb0,0x28,0xe5,0xf5,0x60,0x68,
0x4e,0xef,0x2b,0x74,0x68,0x6b,0x30,0xe5,0x5d,0xa5,0x2b,0xcb,0xb5,0x7a,0x97,0xf7,
0x00,
};


TYPE_GrData _greenGd =
{
	1,		//version
	0,		//reserved
	OSD_1555,		//data format
	COMPRESS_Tfp,		//compressed method
	_greenCpm,		//data
	512,		//size
	16,		//width
	16		//height
};
