#include "StdAfx.h"
#include ".\utils.h"

// This code appears to be in the public domain and as such is compatible with the GPL  - see http://www.gnu.org/philosophy/license-list.html
// Code extracted from the 'unpackhdf_windows' package published on http://humax.roliaforum.com by hrac@address.com which has no copyright notice

/* crc function
 *
 * 2003-02-10T06:47:35Z
 */

static u16 table_crc16[256] = {
	0x0000,0xc0c1,0xc181,0x0140,0xc301,0x03c0,0x0280,0xc241,0xc601,0x06c0,0x0780,0xc741,0x0500,0xc5c1,0xc481,0x0440,
	0xcc01,0x0cc0,0x0d80,0xcd41,0x0f00,0xcfc1,0xce81,0x0e40,0x0a00,0xcac1,0xcb81,0x0b40,0xc901,0x09c0,0x0880,0xc841,
	0xd801,0x18c0,0x1980,0xd941,0x1b00,0xdbc1,0xda81,0x1a40,0x1e00,0xdec1,0xdf81,0x1f40,0xdd01,0x1dc0,0x1c80,0xdc41,
	0x1400,0xd4c1,0xd581,0x1540,0xd701,0x17c0,0x1680,0xd641,0xd201,0x12c0,0x1380,0xd341,0x1100,0xd1c1,0xd081,0x1040,
	0xf001,0x30c0,0x3180,0xf141,0x3300,0xf3c1,0xf281,0x3240,0x3600,0xf6c1,0xf781,0x3740,0xf501,0x35c0,0x3480,0xf441,
	0x3c00,0xfcc1,0xfd81,0x3d40,0xff01,0x3fc0,0x3e80,0xfe41,0xfa01,0x3ac0,0x3b80,0xfb41,0x3900,0xf9c1,0xf881,0x3840,
	0x2800,0xe8c1,0xe981,0x2940,0xeb01,0x2bc0,0x2a80,0xea41,0xee01,0x2ec0,0x2f80,0xef41,0x2d00,0xedc1,0xec81,0x2c40,
	0xe401,0x24c0,0x2580,0xe541,0x2700,0xe7c1,0xe681,0x2640,0x2200,0xe2c1,0xe381,0x2340,0xe101,0x21c0,0x2080,0xe041,
	0xa001,0x60c0,0x6180,0xa141,0x6300,0xa3c1,0xa281,0x6240,0x6600,0xa6c1,0xa781,0x6740,0xa501,0x65c0,0x6480,0xa441,
	0x6c00,0xacc1,0xad81,0x6d40,0xaf01,0x6fc0,0x6e80,0xae41,0xaa01,0x6ac0,0x6b80,0xab41,0x6900,0xa9c1,0xa881,0x6840,
	0x7800,0xb8c1,0xb981,0x7940,0xbb01,0x7bc0,0x7a80,0xba41,0xbe01,0x7ec0,0x7f80,0xbf41,0x7d00,0xbdc1,0xbc81,0x7c40,
	0xb401,0x74c0,0x7580,0xb541,0x7700,0xb7c1,0xb681,0x7640,0x7200,0xb2c1,0xb381,0x7340,0xb101,0x71c0,0x7080,0xb041,
	0x5000,0x90c1,0x9181,0x5140,0x9301,0x53c0,0x5280,0x9241,0x9601,0x56c0,0x5780,0x9741,0x5500,0x95c1,0x9481,0x5440,
	0x9c01,0x5cc0,0x5d80,0x9d41,0x5f00,0x9fc1,0x9e81,0x5e40,0x5a00,0x9ac1,0x9b81,0x5b40,0x9901,0x59c0,0x5880,0x9841,
	0x8801,0x48c0,0x4980,0x8941,0x4b00,0x8bc1,0x8a81,0x4a40,0x4e00,0x8ec1,0x8f81,0x4f40,0x8d01,0x4dc0,0x4c80,0x8c41,
	0x4400,0x84c1,0x8581,0x4540,0x8701,0x47c0,0x4680,0x8641,0x8201,0x42c0,0x4380,0x8341,0x4100,0x81c1,0x8081,0x4040,
};

int crc16(u16 *crc, u8 *buf, uint len)
{
	uint i;
	u16 sum = *crc;
	for(i = 0; i < len; i++) {
		sum = table_crc16[(buf[i] ^ sum) & 255] ^ (sum >> 8);
	}
	*crc = sum;
	return(0);
}

/* uncompress lh5 data
 *
 * 2003-02-10T06:45:39Z
 */

typedef unsigned int uint;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include <string.h>

/* *ah* loads of global variables */

static u32 blocksize;
static u32 control; /* should be in a register */
static int shift; /* should be in a register */

static u8 *srcbuf, *dstbuf;
static uint src, dst;
static uint srclen, dstlen;

#define SHIFT() { if(shift > 0) { control |= (srcbuf[src] << 8 | srcbuf[src + 1]) << shift; src += 2; shift -= 16; } }
#define BITS(cnt) { bits = control >> (32 - (cnt)); shift += (cnt); control <<= (cnt); }

static u16 lit_codes[8192];
static u16 off_codes[1024];
static u8 lit_lens[512];
static u8 off_lens[32];
static u16 freq[17];
static u16 pref[17];

static int tree0(), tree1(), tree2(), tree3(), tree4();
static int huffman_prefixes();
static void general_tree(u16 *codes, uint base_size, u8 *lens, uint symbol);
static void special_tree(u16 *codes, uint base_size, u8 *lens, uint symbol);

int unlh5(u8 *srcpt, uint srcln, u8 *dstpt, uint dstln)
{
	srcbuf = srcpt;
	srclen = srcln + 6; /* little bit of unavoidable overrun */
	dstbuf = dstpt;
	dstlen = dstln;
	src = 0;
	dst = 0;
	control = 0;
	shift = 16;
	while(src < srclen && dst < dstlen) {
		if(tree0() || tree1() || tree2() || tree3() || tree4()) return(1); /* nasty */
	}
	return(dst != dstlen);
}

static int tree0()
{
	uint bits;
	SHIFT();
	BITS(16); blocksize = bits; SHIFT();
	if(src >= srclen) return(1);
	return(blocksize == 0);
}

static int tree1()
{
	uint bits, i, sym, len;
	BITS(5); sym = bits; SHIFT(); /* number of symbols defined in this tree */
	if(src >= srclen) return(1);
	if(sym == 0) {
		BITS(5); sym = bits; SHIFT();
		if(sym >= 20) return(1);
		special_tree(off_codes, 6, off_lens, sym);
		return(0);
	}
	if(sym >= 20) return(1);
	memset(off_lens, 0, sizeof(off_lens));
	memset(freq, 0, sizeof(freq));
	for(i = 0; i < sym; i++) {
		if(src >= srclen) return(1);
		BITS(3); len = bits;
		while(len >= 7) {
			BITS(1); if(bits == 0) break;
			len++;
			if(len >= 15) return(1);
		}
		SHIFT();
		off_lens[i] = len; freq[len]++;
		if(i == 2) { BITS(2); i += bits; SHIFT(); }
	}
	if(i != sym) return(1);
	if(huffman_prefixes()) return(1);
	memset(off_codes, 0, sizeof(off_codes));
	general_tree(off_codes, 6, off_lens, 20);
	return(0);
}

static int tree2()
{
	uint bits, i, sym, tmp;
	BITS(9); sym = bits; SHIFT(); /* number of symbols defined in this tree */
	if(src >= srclen) return(1);
	if(sym == 0) {
		BITS(9); sym = bits; SHIFT();
		if(sym >= 511) return(1);
		special_tree(lit_codes, 12, lit_lens, sym);
		return(0);
	}
	if(sym >= 511) return(1);
	memset(lit_lens, 0, sizeof(lit_lens));
	memset(freq, 0, sizeof(freq));
	for(i = 0; i < sym; i++) {
		if(src >= srclen) return(1);
		tmp = off_codes[control >> 26];
		if(tmp >= 64) {
			tmp = off_codes[tmp + (control >> 22 & 15)];
			if(tmp >= 64) {
				tmp = off_codes[tmp + (control >> 18 & 15)];
			}
		}
		shift += off_lens[tmp]; control <<= off_lens[tmp];
		SHIFT();
		switch(tmp) {
		case 0: break; /* zero */
		case 1: BITS(4); i += bits + 2; SHIFT(); break; /* zero 3-18 */
		case 2: BITS(9); i += bits + 19; SHIFT(); break; /* zero 20-531 */
		default: tmp -= 2; lit_lens[i] = tmp; freq[tmp]++; break;
		}
	}
	if(i != sym) return(1);
	if(huffman_prefixes()) return(1);
	memset(lit_codes, 0, sizeof(lit_codes));
	general_tree(lit_codes, 12, lit_lens, 510);
	return(0);
}

static int tree3()
{
	uint bits, i, sym, len;
	BITS(4); sym = bits; SHIFT(); /* number of symbols defined in this tree */
	if(src >= srclen) return(1);
	if(sym == 0) {
		BITS(4); sym = bits; SHIFT();
		if(sym >= 15) return(1);
		special_tree(off_codes, 8, off_lens, sym);
		return(0);
	}
	if(sym >= 15) return(1);
	memset(off_lens, 0, sizeof(off_lens));
	memset(freq, 0, sizeof(freq));
	for(i = 0; i < sym; i++) {
		if(src >= srclen) return(1);
		BITS(3); len = bits;
		while(len >= 7) {
			BITS(1); if(bits == 0) break;
			len++;
			if(len >= 17) return(1);
		}
		SHIFT();
		off_lens[i] = len; freq[len]++;
	}
	if(i != sym) return(1);
	if(huffman_prefixes()) return(1);
	memset(off_codes, 0, sizeof(off_codes));
	general_tree(off_codes, 8, off_lens, 15);
	return(0);
}

static int tree4()
{
	uint lit, len, off;
	while(blocksize--) {
		if(src >= srclen || dst >= dstlen) return(1);
		lit = lit_codes[control >> 20];
		if(lit >= 4096) {
			lit = lit_codes[lit + (control >> 16 & 15)];
		}
		shift += lit_lens[lit]; control <<= lit_lens[lit];
		SHIFT();
		if(lit < 256) { dstbuf[dst++] = lit; continue; }
		len = off_codes[control >> 24];
		if(len >= 256) {
			len = off_codes[len + (control >> 20 & 15)];
			if(len >= 256) {
				len = off_codes[len + (control >> 16 & 15)];
			}
		}
		shift += off_lens[len]; control <<= off_lens[len];
		SHIFT();
		if(len < 2) {
			off = len + 1;
		} else {
			len--;
			off = (1u << len | control >> (32 - len)) + 1;
			shift += len; control <<= len;
			SHIFT();
		}
		if(off > dst) return(1);
		off = dst - off;
		lit -= 253;
		while(lit--) dstbuf[dst++] = dstbuf[off++];
	}
	return(0);
}

static int huffman_prefixes()
{
	uint i;
	uint base = 0, left = 1;
	for(i = 1; i < 17; i++) {
		base *= 2; left *= 2;
		pref[i] = base;
		base += freq[i]; left -= freq[i];
	}
	return(left != 0);
}

static void special_tree(u16 *codes, uint base_size, u8 *lens, uint symbol)
{
	uint i;
	for(i = 0; i < (1u << base_size); i++) codes[i] = symbol; lens[symbol] = 0;
}

static void general_tree(u16 *codes, uint base_size, u8 *lens, uint symbol)
{
	uint i;
	uint len, code, base, bits;
	uint alloc;
	alloc = 1u << base_size;
	for(i = 0; i < symbol; i++) {
		len = lens[i];
		if(len == 0) continue;
		code = pref[len]++;
		base = 0;
		bits = base_size;
		while(len > bits) {
			len -= bits;
			base += code >> len;
			code &= (1u << len) - 1;
			bits = 4;
			if(codes[base] == 0) {
				codes[base] = alloc; alloc += 1u << bits;
			}
			base = codes[base];
		}
		if(len < bits) {
			bits -= len;
			base += code << bits;
			for(len = 1u << bits; len--; base++) codes[base] = i;
		} else {
			codes[base + code] = i;
		}
	}
}
