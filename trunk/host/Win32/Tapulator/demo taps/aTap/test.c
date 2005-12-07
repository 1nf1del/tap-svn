#include <tap.h>

byte _smallgreenbarCpm[] = 
{
	0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x01,0xe8,0x58,0xe8,0xff,0xff,
	0x30,0x72,0x01,0x49,0x63,0x73,0xad,0xa7,0x7f,0xc9,0x5b,0xc0,0xa2,0x70,0x2b,0x5e,
	0x05,0x6b,0xc0,0xad,0x9d,0x66,0x03,0x08,0xc3,0x30,0x9f,0xfa,0xa3,0x0c,0xc2,0xb0,
	0xcc,0x2b,0x0a,0xc0,0x32,0xd7,0x81,0x41,0xc0,0x8c,0xa8,0x42,0x49,0x24,0x92,0x59,
	0x6d,0x36,0x0c,0x20,0xcb,0x6f,0xa5,0xf4,0xbe,0xc3,0xf9,0x8f,0xcf,0xa4,0x0a,0x9c,
	0x25,0xc6,0xe1,0x41,0xc3,0x3f,0x50,0x5a,0xc6,0xd3,0xd2,0x78,0x01,0xc0,0xa2,0xf0,
	0x17,0x18,0xb3,0x79,0xc4,0xbe,0xa6,0x28,0x3f,0x94,0x75,0x75,0x3b,0xd3,0xd1,0x35,
	0x97,0x28,0xc1,0x5f,0x05,0x0e,0x6e,0x55,0xa1,0xb8,0xaa,0x10,0x5d,0x8f,0xbb,0x4d,
	0xa1,0x29,0xb4,0x37,0x31,0x73,0xda,0x9a,0x71,0x40,0x00,0x00,0x73,0x30,0xe3,0xc3,
	0x8e,0x7c,0x78,0x73,0x06,0xe2,0xa1,0x54,0x33,0xdb,0x35,0xf1,0xe7,0xab,0xbc,0xfb,
	0x58,0x7f,0xc4,0xeb,0x6d,0x9f,0xcf,0x19,0xd4,0xfe,0x6c,0x36,0x7f,0x87,0x10,0x00,
	0xfa,0x3c,0x7f,0x84,0xcb,0xe7,0x37,0xcd,0xa9,0xba,0xcd,0xa6,0x8f,0x5c,0x5f,0xd5,
	0xdb,0x32,0x79,0xbd,0x53,0x66,0x3e,0x0b,0x35,0x79,0x5c,0xcd,0xdf,0x72,0xc3,0x03,
	0x79,0xf8,0x00,0x0e,0x47,0x7f,0x50,0x00,0x72,0x25,0xe9,0x00,0x0f,0xb3,0xa0,0x0e,
	0x07,0x75,0x21,0x7d,0xbf,0xad,0x87,0xbd,0x1f,0xfc,0x97,0x37,0x99,0x6d,0x73,0x97,
	0x76,0x0b,0x2d,0x50,0x78,0xe8,0xf7,0x80,0xe2,0x0b,0x6c,0x71,0x57,0x4a,0xf8,0xa2,
	0xd2,0xca,0xea,0xfd,0x17,0xbb,0x11,0x6e,0x43,0xf6,0x9b,0xaf,0xf0,0xb3,0xb8,0xe0,
	0x83,0x04,0x34,0x88,0xf7,0x02,0x60,0x96,0x1e,0x52,0x13,0xfb,0x55,0x4c,0x0b,0x0d,
	0x7b,0x03,0x7d,0xc0,0x94,0xf7,0xae,0xc2,0x98,0x30,0xa6,0x0d,0x98,0x6c,0x0b,0xb1,
	0x73,0x3b,0xa6,0xc2,0x95,0x9b,0x2e,0xd8,0xf2,0xaa,0xf0,0x37,0x92,0x64,0x28,0x4a,
	0x49,0x32,0x2a,0x09,0x78,0x97,0xcb,0x38,0xc8,0xbb,0x16,0x4b,0x23,0x69,0xc8,0x9a,
	0x12,0x56,0x96,0x58,0xa4,0x3f,0x17,0x60,0x2a,0x6f,0x25,0x58,0x5b,0x38,0xea,0x92,
	0x44,0xb2,0xab,0x3d,0x23,0x7f,0x21,0x56,0x42,0x06,0xe2,0x80,0x38,0x1d,0xa0,0x5c,
	0x7e,0x40,0x03,0x90,0x3e,0xb0,0x00,0x03,0x99,0x7e,0x31,0xc7,0x7e,0x90,0x79,0xf8,
	0xb0,0x1d,0xbc,0xff,0xa0,0xf3,0xdf,0x8d,0x3d,0x59,0x37,0xd1,0xb4,0x76,0xc9,0x1f,
	0xed,0xff,0x43,0x4e,0x86,0x11,0x9f,0x31,0xd9,0x76,0x06,0xe2,0x2f,0x20,0xbe,0x51,
	0x0c,0xa3,0xba,0xb2,0xfe,0x33,0xdf,0x9b,0xcb,0xac,0xde,0xd3,0x1e,0x7a,0x33,0x5b,
	0xb2,0x3b,0x3d,0xc6,0xd6,0x98,0xd7,0x6f,0x65,0x6c,0x4d,0x6d,0x89,0x7a,0x0c,0x56,
	0xf5,0x76,0x22,0xaf,0xc5,0x40,0xf9,0x3b,0x00,0xb8,0x17,0x90,0x6e,0x65,0xee,0x5f,
	0xa6,0x8e,0xeb,0x33,0x51,0x89,0x22,0x94,0xd9,0xd5,0xf8,0x0b,0xa8,0xc4,0x79,0x2e,
	0x69,0x8a,0xaa,0xb5,0x5a,0xa1,0xec,0x4d,0xb5,0x15,0x6c,0x36,0x60,0xa8,0xbb,0x12,
	0xd9,0xc0,0xe2,0x00
};
TYPE_GrData _smallgreenbarGd = { 1, 0, OSD_1555, COMPRESS_Tfp, _smallgreenbarCpm, 12402, 159, 39 };

