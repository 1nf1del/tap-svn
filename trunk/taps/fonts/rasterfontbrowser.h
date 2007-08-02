#ifndef RASTERFONTBROWSER_H
#define RASTERFONTBROWSER_H

#include "localtap.h"
#include <stdio.h>


#ifdef _WIN32
typedef unsigned long  dword;
typedef unsigned int   uint;
typedef unsigned short word;
typedef unsigned char  byte;

#define RGB(r,g,b)      ( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
#define ARGB(a,r,g,b)   ( (a<<15) | ((r)<<10) | ((g)<<5) | (b))
#define RGB8888(r,g,b)  RGB( (r>>3), (g>>3), (b>>3) )
#endif

#define GETR(rgb)       ((byte)((rgb)>>10)&0x1F)
#define GETG(rgb)       ((byte)(((word)(rgb))>>5)&0x1F)
#define GETB(rgb)       ((byte)(rgb&0x1F))

#define PUTPIXEL_OR(IMAGE, X, Y, COLOR)		IMAGE[Y*width+X]|= COLOR
#define PUTPIXEL_SET(IMAGE, X, Y, COLOR)	IMAGE[Y*width+X] = COLOR
#define GETPIXEL(IMAGE, X, Y, COLOR)		COLOR = IMAGE[Y*width+X]

typedef struct
{
	long  slot_advance_x;
	long  slot_advance_y;
	long  slot_bitmap_left;
	long  slot_bitmap_top;
	long  bitmap_rows;
	long  bitmap_width;
	long  offset;
} TableItem;

typedef struct
{
	char *m_data;

	word           m_foreColor;
	word           m_backColor;
	word           m_antialiasmap[16];
	TableItem     *m_bmpHeaderArray;
	long		   m_fontHeight;
	long		   m_fontBaseline;
	int			   m_bShifted;
} FONT;

int Load_Font(FONT *font, char *fileName);
void Delete_Font(FONT *font);
int TAP_Osd_PutS_Font(word rgn, dword x, dword y, dword maxX, char *str, word fcolor, word bcolor, FONT *font, byte align);
int TAP_Osd_PutS_FontEx(word rgn, dword x, dword y, dword maxX, dword maxY, int baseline_shift, char *str, word fcolor, word bcolor, FONT *font, byte align);
int TAP_Osd_PutS_FontL(word rgn, dword x, dword y, dword maxX, char *str, FONT *font, byte align);
int Draw_Font_String(word rgn, dword x, dword y, char *str, char* fontname,  word foreColor, word backColor);

void SetColors(FONT *font, word foreColor, word backColor);

#define LOAD_CHAR(font, pos, bitmap)	*(bitmap) = (font)->m_data + (font)->m_bmpHeaderArray[(pos)].offset;

#endif // RASTERFONTBROWSER_H