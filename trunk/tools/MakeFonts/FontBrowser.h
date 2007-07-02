#ifndef FONTBROWSER_H
#define FONTBROWSER_H

extern "C" 
{
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
}

#ifdef _WIN32
typedef unsigned long dword;
typedef unsigned int uint;
typedef unsigned short	word;
typedef unsigned char byte;

#define RGB(r,g,b)	( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
#define ARGB(a,r,g,b)	( (a<<15) | ((r)<<10) | ((g)<<5) | (b))
#define RGB8888(r,g,b)	RGB( (r>>3), (g>>3), (b>>3) )
#endif

#define GETR(rgb)       ((byte)((rgb)>>10)&0x1F)
#define GETG(rgb)       ((byte)(((word)(rgb))>>5)&0x1F)
#define GETB(rgb)       ((byte)(rgb&0x1F))

#define PUTPIXEL(X, Y, COLOR)  image[Y*width+X]|=COLOR
#define GETPIXEL(X, Y, COLOR)  COLOR = image[Y*width+X]


class FontBrowser
{
public:	
	FontBrowser();
	~FontBrowser();

	int SetFontName(char* fontName);
	int SetFontHeight(int* height);
	int SetCharMap(char* charMapName); //  http://czyborra.com/charsets/
	void SetRenderMode(int mode);	   // FT_RENDER_MODE_NORMAL = 0,	FT_RENDER_MODE_MONO==2
	void SetColors(word foreColor, word backColor);

	int CalcSize(unsigned char* text, int* width, int* height, int* text_length);
	int CalcSizeA(char* fontName, unsigned char* text, int* width, int* height, int* text_length);

	int DisplayString(unsigned char* text, int text_length, int* width, int* height, word** image);
	int DisplayStringA(char* fontName, unsigned char* text, int* width, int* height, word** image);

	FT_Error       init_err;
	FT_Library     m_library;
	FT_Face        m_face;
	FT_Render_Mode m_renderMode;
	int            m_vert_offset;
	word           m_foreColor, m_backColor;
	unsigned int  *m_charmap;
	word          *m_antialiasmap;

	void draw_bitmap(FT_Bitmap*  bitmap, word* image, int width, int height, FT_Int x_offset, FT_Int y_offset);
	void FillImage(word* image, int width, int height);
	int ConvertString(unsigned char* src, unsigned int *dest, int text_length);
	void ComputeAntialiasingMap(word foreColor, word backColor);

};

#endif // FONTBROWSER_H