#pragma once

typedef unsigned int uint;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

int crc16(u16 *crc, u8 *buf, uint len);
int unlh5(u8 *srcpt, uint srcln, u8 *dstpt, uint dstln);

