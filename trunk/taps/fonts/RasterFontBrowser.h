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
	unsigned char *m_data;

	word           m_foreColor;
	word           m_backColor;
	word           m_antialiasmap[16];
	TableItem     *m_bmpHeaderArray;
	long		   m_fontHeight;
} FONT;

int Load_Font(FONT *font, char *fileName);
void Delete_Font(FONT *font);

int Load_Font_Height(FONT *font);
int Load_Char_Table(FONT* font);
void SetColors(FONT *font, word foreColor, word backColor);
void FillImage(FONT *font, word *image, int width, int height);
void ComputeAntialiasingMap(FONT *font, word foreColor, word backColor);
int CalcSize(FONT *font, unsigned char* text, int* width, int *text_length);
int RenderString(FONT *font, unsigned char* text, int text_length, int width, word** image);
void draw_bitmap(FONT *font, unsigned char* bitmap, unsigned char *image, int width, int height, int x_offset, int y_offset, int bitmap_rows, int bitmap_width);
int Draw_Font_String(word region, dword x, dword y, unsigned char *text, unsigned char* fontname,  word foreColor, word backColor);

void InitDirect();
void FillRectDirect(int x, int y, int width, int height, word color);
int RenderStringDirect(FONT *font, unsigned char* text, int text_length, int x, int y, int width);
void draw_bitmap_direct(FONT *font, unsigned char* bitmap, int width, int height, int x_offset, int y_offset, int bitmap_rows, int bitmap_width);


#define LOAD_CHAR(font, pos, bitmap)	*(bitmap) = (font)->m_data + (font)->m_bmpHeaderArray[(pos)].offset;


#endif // RASTERFONTBROWSER_H