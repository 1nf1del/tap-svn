#include "type.h"
byte _green27x27Cpm[] = 
{
0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0xef,0xbc,0x24,0xff,0xff,
0x05,0xb2,0x00,0xa6,0x5b,0x53,0x8e,0x29,0xfe,0xb8,0x0b,0xa3,0x3f,0x80,0x0d,0xc0,
0x5d,0xc0,0x5e,0xed,0x01,0x01,0x03,0x80,0x44,0x42,0xbf,0x3c,0x4f,0x01,0x21,0x07,
0x63,0x3b,0x00,0xf4,0x25,0x39,0x0f,0x6e,0xdb,0xdd,0xfd,0x27,0xaa,0x0a,0xd9,0xc0,
0x74,0x9c,0xc6,0x38,0x1c,0xfc,0x48,0x78,0x0d,0xd8,0x00,0x05,0xb7,0x59,0xef,0xdf,
0xde,0x10,0x20,0x28,0xbc,0x1e,0xd2,0x32,0x68,0x87,0x94,0xc9,0xcf,0x1b,0xf9,0xca,
0xdf,0x84,0xcd,0x0b,0x87,0xea,0xaf,0x61,0x06,0x72,0x78,0x3e,0x1a,0x7e,0x68,0xcc,
0x95,0x04,0x56,0x23,0x1c,0x6c,0xf9,0xa7,0xae,0xdd,0xd6,0x6b,0xb5,0x57,0x72,0x74,
0xa5,0x71,0x28,0x0b,0x72,0x94,0xec,0xe1,0xc7,0x96,0xdf,0x8a,0x2b,0x3a,0x84,0xc8,
0x44,0xfb,0x2a,0xf6,0x2d,0x2b,0x0d,0x5b,0xfe,0xf4,0x59,0xf5,0x7d,0x43,0x29,0x4a,
0x2b,0x5c,0x26,0xfb,0xb1,0xec,0x30,0xb5,0x67,0x29,0x81,0x38,0xbb,0xd4,0x29,0x17,
0xeb,0x9d,0x67,0x69,0xa4,0xa4,0x2a,0xaa,0x6d,0x97,0x61,0xd7,0x26,0x6a,0x98,0x56,
0xa1,0x20,0x7f,0xe7,0x37,0x04,0xdd,0x29,0x4d,0x70,0x68,0xb9,0x4b,0x9f,0x32,0x96,
0x7d,0x13,0x20,0xae,0x79,0x49,0x6a,0x9a,0xb2,0x38,0xf3,0x4c,0xf2,0xf8,0x75,0x0d,
0x3a,0x23,0x34,0xe5,0x4e,0xc7,0x3f,0xa2,0x59,0x43,0xe7,0x25,0x6e,0x36,0x8f,0x4f,
0x96,0x44,0xd4,0xf4,0xc6,0xe2,0x7e,0xbf,0x29,0xff,0xf0,
};


TYPE_GrData _green27x27Gd =
{
	1,		//version
	0,		//reserved
	OSD_1555,		//data format
	COMPRESS_Tfp,		//compressed method
	_green27x27Cpm,		//data
	1458,		//size
	27,		//width
	27		//height
};
