#include "tap.h"

static byte _bigkeyblueCpm[] = 
{
	0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0x60,0x98,0x26,0xff,0xff,
	0x22,0x44,0x00,0x32,0x4b,0x48,0xa2,0xd5,0x7f,0xc8,0x8f,0xe9,0x13,0xfb,0x45,0x44,
	0x45,0x48,0xc6,0xdd,0xd2,0x2d,0x92,0xff,0x80,0x80,0x91,0x2f,0x38,0x20,0x00,0x00,
	0x80,0x02,0xfe,0xda,0xaf,0xf5,0x51,0x16,0x24,0xe3,0x35,0x6d,0x37,0xa9,0x3b,0x4d,
	0xf2,0x4c,0x4d,0x25,0x92,0xc9,0x64,0xb2,0x59,0x2c,0x96,0x4b,0x25,0x92,0xc9,0x64,
	0xb2,0x59,0x2c,0x96,0x4b,0x25,0x92,0xc9,0x64,0xb2,0x59,0x2c,0x96,0x4b,0x25,0x92,
	0xc9,0x64,0xb7,0xaf,0xaf,0xc5,0x78,0x93,0x13,0x6a,0x88,0xa0
};
static TYPE_GrData _bigkeyblueGd = { 1, 0, OSD_1555, COMPRESS_Tfp, _bigkeyblueCpm, 8772, 102, 43 };

static byte _bigkeygreenCpm[] = 
{
	0x00,0x08,0x1b,0x90,0xff,0xff,0x00,0x01,0x00,0x01,0x00,0x60,0xc7,0x36,0xff,0xff,
	0x22,0x44,0x00,0x32,0x4b,0x68,0x82,0x45,0x3f,0xc1,0xbb,0xe9,0xfd,0x36,0x44,0x00,
	0xa0,0x09,0x05,0x8c,0xd8,0x82,0xd4,0x2f,0xf5,0x06,0x82,0x09,0x73,0x82,0x00,0x00,
	0x08,0x00,0x2b,0x5f,0x57,0xf8,0xaa,0x22,0xc4,0x9f,0xb3,0x56,0xd3,0x7a,0x93,0xb4,
	0xdf,0x24,0xc4,0xd2,0x59,0x2c,0x96,0x4b,0x25,0x92,0xc9,0x64,0xb2,0x59,0x2c,0x96,
	0x4b,0x25,0x92,0xc9,0x64,0xb2,0x59,0x2c,0x96,0x4b,0x25,0x92,0xc9,0x64,0xb2,0x59,
	0x2c,0x96,0x4b,0x7a,0xfa,0xfc,0x57,0x89,0x31,0x36,0xa8,0x8a
};
static TYPE_GrData _bigkeygreenGd = { 1, 0, OSD_1555, COMPRESS_Tfp, _bigkeygreenCpm, 8772, 102, 43 };

static byte _popup360x180Cpm[] = 
{
	0x00,0x08,0x18,0x50,0xff,0xff,0x00,0x01,0x00,0x04,0x00,0xbc,0xdc,0xc0,0xff,0xff,
	0x7f,0xfa,0x00,0x8f,0x4c,0x4c,0xc0,0x6a,0x7f,0xde,0xc6,0xa6,0x0c,0xaa,0x60,0xfb,
	0x03,0x53,0xa1,0x4c,0x9d,0x80,0x4c,0x25,0x40,0xc0,0xb4,0xde,0xa1,0x02,0x0f,0x77,
	0xfd,0xee,0x40,0xe7,0x22,0x60,0x5f,0xdd,0x48,0x00,0x60,0x1a,0xb7,0x4b,0x1e,0x5f,
	0x60,0x6e,0x96,0x9c,0x63,0x92,0x93,0x55,0x8b,0xca,0x4d,0x36,0x27,0x22,0x69,0xb1,
	0x09,0x13,0x47,0x0f,0xa3,0x6d,0xc1,0xb3,0x39,0xa2,0x99,0x24,0xe6,0xae,0x53,0x35,
	0x8c,0xce,0x26,0x9a,0x66,0x9f,0xcd,0x14,0xcd,0x2b,0x34,0x53,0x34,0xe6,0x68,0xa6,
	0x69,0xf4,0xd1,0x4c,0xd3,0xa9,0xa2,0xeb,0x3a,0x79,0xd6,0x85,0x33,0x4a,0xf5,0xa1,
	0x4c,0xf2,0x26,0x7b,0xc1,0xb4,0xf7,0xff,0x05,0x4c,0xd3,0xb9,0xa2,0xc2,0x62,0x67,
	0x82,0x67,0x82,0x67,0x82,0x67,0x89,0x9e,0x99,0xe2,0x67,0x82,0x67,0x82,0x67,0x82,
	0x67,0x82,0x67,0x89,0x9e,0x09,0x9e,0x09,0x9e,0x09,0x9e,0x09,0x9e,0x26,0x7b,0x09,
	0x89,0x9e,0x09,0x9e,0x09,0x9e,0x09,0x9e,0x26,0x7a,0x67,0x89,0x9e,0x09,0x9e,0x09,
	0x9e,0x09,0x9e,0x09,0x9e,0x26,0x79,0xa8,0x00,0x85,0x2f,0xb1,0xff,0xff,0x7f,0xfa,
	0x00,0x88,0x43,0x88,0x80,0x25,0xfe,0x0d,0x77,0x6d,0xb2,0x01,0x6e,0xa4,0x0f,0xa0,
	0x2d,0x06,0x70,0x4b,0x40,0x0c,0x06,0x6d,0xf6,0x08,0x00,0x00,0x12,0xe4,0x9d,0x56,
	0xac,0x84,0xcf,0x6c,0xfd,0xc0,0x61,0xb3,0xc6,0xcf,0x06,0xcf,0x06,0xcf,0x06,0xcf,
	0x06,0xcf,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x6c,0xf6,0xcf,0x1b,
	0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x6c,0xf0,0x6c,0xf0,0x6c,0xf0,0x6c,
	0xf0,0x6c,0xf1,0xb3,0xc1,0xb3,0xc1,0xb3,0xc1,0xb3,0xc1,0xb3,0xc6,0xcf,0x6c,0xf1,
	0xb3,0xc1,0xb3,0xc1,0xb3,0xc1,0xb3,0xc1,0xb3,0xc6,0xcf,0x06,0xcf,0x06,0xcf,0x06,
	0xcf,0x06,0xcf,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0xfe,0xcf,0x00,
	0x84,0x8a,0xa2,0xff,0xff,0x7f,0xfa,0x00,0x88,0x43,0x48,0x80,0x29,0xfe,0x4d,0x7e,
	0x6f,0xed,0x24,0x78,0xeb,0x42,0x48,0x06,0x50,0x32,0x31,0xeb,0x04,0x00,0x00,0x09,
	0x4f,0xa9,0xf8,0x0e,0x5e,0xc1,0xc2,0x13,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,
	0x6c,0xf6,0xcf,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x1b,0x3c,0x6c,0xf0,0x6c,
	0xf0,0x6c,0xf0,0x6c,0xf0,0x6c,0xf1,0xb3,0xd0,0x98,0xd9,0xe0,0xd9,0xe0,0xd9,0xe0,
	0xd9,0xe3,0x67,0xb6,0x78,0xd9,0xe0,0xd9,0xe0,0xd9,0xe0,0xd9,0xe0,0xd9,0xe3,0x67,
	0x83,0x67,0x83,0x67,0x83,0x67,0x83,0x67,0x8d,0x9e,0x84,0xc6,0xcf,0x06,0xcf,0x06,
	0xcf,0x06,0xcf,0x1b,0x3d,0xb3,0xc6,0xcf,0x06,0xcf,0x06,0xcf,0x06,0xcf,0x06,0xcf,
	0x1b,0x3c,0x1b,0x3c,0xd0,0x00,0xc2,0xf6,0x67,0xff,0xff,0x7a,0x52,0x00,0x92,0x53,
	0x50,0xc3,0x50,0x97,0xfb,0xfe,0x35,0x46,0xbb,0x5c,0x47,0x78,0x0e,0x80,0x85,0x40,
	0xb9,0x78,0x40,0xf0,0xc5,0x29,0x0a,0x12,0xd0,0xce,0x42,0x73,0x9d,0xed,0x05,0x6b,
	0x81,0xff,0x75,0x20,0x00,0x00,0x8a,0x59,0x67,0x73,0x35,0xbd,0xaf,0x30,0x4c,0x4c,
	0xf0,0x4c,0xf0,0x4c,0xf0,0x4c,0xf1,0x33,0xd3,0x3c,0x4c,0xf0,0x4c,0xf0,0x4c,0xf0,
	0x4c,0xf0,0x4c,0xf1,0x33,0xc1,0x33,0xc1,0x33,0xc1,0x33,0xc1,0x33,0xc4,0xcf,0x60,
	0x98,0x99,0xe0,0x99,0xe0,0x99,0xe0,0x99,0xe2,0x67,0xa6,0x78,0x99,0xe0,0x99,0xe0,
	0x99,0xe0,0x99,0xe1,0x8c,0xcf,0x4d,0x13,0x79,0x9b,0x22,0x67,0x9c,0x4c,0xf9,0x13,
	0x3c,0xff,0x33,0xd3,0x44,0xdb,0xe1,0xb5,0xad,0x09,0xd6,0xb3,0xad,0x68,0x4a,0xd6,
	0x75,0x34,0x4a,0x99,0xb7,0x4c,0xf4,0xd1,0x3d,0xf3,0x3a,0xc8,0x9a,0x27,0xe9,0x9a,
	0x9a,0x25,0x4c,0xdb,0xe6,0x8a,0x69,0x9a,0xf7,0x6e,0xcf,0x3e,0x77,0x82,0x9e,0xa6,
	0x4e,0x13,0x37,0xc4,0xd1,0x0d,0x13,0x2f,0x30,0x84,0xe6,0x66,0xa6,0x99,0x53,0x2d,
	0x34,0xcf,0xb3,0x2d,0x35,0x4f,0x9a,0xca,0x80
};
static TYPE_GrData _popup360x180Gd = { 1, 0, OSD_1555, COMPRESS_Tfp, _popup360x180Cpm, 129600, 360, 180 };

