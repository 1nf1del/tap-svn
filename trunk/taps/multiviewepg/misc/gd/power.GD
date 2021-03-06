#include "type.h"
byte _powerCpm[] = 
{
0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0xfa,0xc4,0x0c,0xff,0xff,
0x06,0x1c,0x00,0xab,0x5b,0x57,0xad,0xa9,0xfe,0xfe,0x0d,0x13,0x84,0xbe,0x0a,0xe3,
0x30,0x9d,0xe0,0x8e,0x02,0x81,0x80,0xc2,0x51,0x30,0x98,0x8c,0x42,0x37,0x4a,0x13,
0x28,0x58,0x4d,0x30,0x98,0x44,0x44,0x0b,0xaf,0xff,0x79,0x8a,0xb1,0x75,0xd6,0xe5,
0x56,0x32,0x31,0x21,0x7c,0xbe,0xd7,0x18,0xda,0xb4,0xdb,0x0c,0xfb,0xef,0x4b,0x13,
0x97,0xc7,0xb9,0x82,0x90,0x48,0x5a,0x1b,0x01,0x7b,0xd5,0xa0,0xa2,0xf2,0x9c,0x3d,
0x84,0xb5,0xf6,0xa0,0xbb,0x8b,0xcb,0xd2,0x82,0x6f,0x00,0xed,0x7f,0x57,0x26,0xdb,
0xf4,0x33,0xf3,0xa8,0x3d,0x3d,0x01,0x65,0x8a,0xae,0x51,0xc9,0x23,0xdf,0xa9,0x95,
0x89,0x99,0x22,0xe3,0x78,0x2d,0x10,0x01,0x53,0x99,0x7f,0x86,0xc2,0x7f,0xad,0x92,
0x17,0xdd,0x8f,0x05,0xd7,0x84,0x13,0x2c,0x49,0xc4,0x9c,0x4c,0x3f,0xab,0x8c,0xa0,
0x30,0x5c,0x63,0x63,0x10,0xf8,0x27,0x98,0x1a,0x21,0x38,0x88,0xdf,0x3e,0xef,0xd9,
0x41,0x66,0x77,0xbe,0xec,0x00,0xf4,0xfb,0x03,0x43,0xc0,0xd1,0x85,0x0d,0x4e,0x40,
0x66,0x1a,0x16,0x7d,0xb6,0x74,0x45,0xd6,0x7d,0x38,0xd9,0x7c,0xa4,0xb5,0x9d,0xb5,
0x2d,0xb6,0x5c,0x45,0xd0,0x2e,0x57,0x66,0x16,0x55,0xad,0x71,0xdf,0xa8,0xac,0xca,
0x38,0x4a,0x7b,0x72,0xb6,0x7e,0xd8,0xc3,0x0c,0x1d,0x82,0xb4,0xa3,0x1f,0x59,0xd2,
0x78,0xa9,0x26,0x5e,0xef,0xb5,0x79,0xce,0x17,0xed,0x4a,0x0a,0x6f,0xd3,0xbd,0x23,
0xfe,0x3b,0xf9,0xb6,0x39,0xf0,
};


TYPE_GrData _powerGd =
{
	1,		//version
	0,		//reserved
	OSD_1555,		//data format
	COMPRESS_Tfp,		//compressed method
	_powerCpm,		//data
	1564,		//size
	34,		//width
	23		//height
};
