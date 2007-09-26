extern "C" 
{
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
}

#include <stdio.h>
#include "FontBrowser.h"

extern "C" 
{
#include "RasterFontBrowser.h"
}

#define htonl(A) ((((long)(A) & 0xff000000) >> 24) | (((long)(A) & 0x00ff0000) >> 8) | (((long)(A) & 0x0000ff00) << 8) | (((long)(A) & 0x000000ff) << 24))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b)) 


#ifdef CHECK_ENDIANNES
void check_endiannes()
{
	union {
		short s;
		char c[sizeof(short)];
	} un;
	un.s = 0x0102;
	if(sizeof(short) == 2)
	{
		if(un.c[0] == 1 && un.c[1] == 2)
			printf("big-endian\n");
		else if(un.c[0] == 2 && un.c[1] == 1)
			printf("little-endian\n");
		else
			printf("unknown\n");
	}
	else
	{
		printf("sizeof(short) = %d\n", sizeof(short));
	}
}
#endif


#define GET_BIT(BUF, POS) ((((unsigned char *)(BUF))[POS>> 3] & (1 << (7 - (POS & 0x7)))) ? 1 : 0)
int get_bit(unsigned char *buf, unsigned int pos)
{
	return (((unsigned char*)(buf))[pos >> 3] & (1 << (7 - (pos	& 0x7)))) ? 1 : 0;
}


int main(int argc, char* argv[])
{
	if (argc<5)
	{
		printf("Usage: rasterizer.exe font_file charmap.txt glyph_height outputFile\n");
		return -1;
	}


	FontBrowser *fb;
	FT_Error err;
	char* fontName = argv[1];
	char* charmapName = argv[2];
	int height = atoi(argv[3]);
	char* outputName = argv[4];
	printf("Raterizing:\n\tFont: %s\n\tCharmap: %s\n\tHeight: %d\n\tOutput: %s\n", fontName, charmapName, height, outputName);

	fb = new FontBrowser();
	if (fb->init_err!=0)
	{
		delete fb;
		return fb->init_err;
	}

	err=fb->SetFontName(fontName);
	if (err!=0)
	{
		switch(err)
		{
		case -1: printf("The font file could be opened and read, but its font format is unsupported.\n");
			break;
		case -2: printf("The font file could not be opened or read, or it is broken.\n");
			break;
		case -3: printf("Unknown error\n");
			break;
		}

		delete fb;
		return err;
	}
	
	fb->SetRenderMode(FT_RENDER_MODE_NORMAL);
	fb->SetColors(RGB(255,255,255), RGB(0,0,0));

	err=fb->SetCharMap(charmapName);
	if (err!=0)
	{
		printf("Character map file not found.\n");
		delete fb;
		return err;
	}

	err=fb->SetFontHeight(&height);
	if (err!=0)
	{
		printf("FreeType2 library was not initialized.\n");
		delete fb;
		return err;
	}

	//////////////////////////////////////////////////////////////////////////
	//   0. Calculate baseline & height
	//////////////////////////////////////////////////////////////////////////
	int baseline=0, new_height=0; 
	fb->CalculateBaseline(&baseline, &new_height);


//#ifdef TEST_LIB_RENDER
//	int width = 0;
//	word *image=NULL;
//	unsigned char text[]="qUIck";
//	int text_length=(int)strlen((char*)text);
//
//	fb->m_baseline = baseline;
//	fb->CalcSize(text, &width, &height, &text_length);
//	fb->DisplayString(text, text_length, &width, &height, &image);
//
//	for (int y=0; y<height; y++)
//	{
//		for (int x=0; x<width; x++)
//		{
//			if (image[y*width+x]>RGB(0,0,0)) printf("0");
//			else printf(".");
//		}
//		printf("\n");
//	}
//	free(image);
//#endif                


	//////////////////////////////////////////////////////////////////////////
	//   1. Create 0..256 alphabet
	//////////////////////////////////////////////////////////////////////////
	int i;
	unsigned char src[256];
	unsigned int dest[256];

	for (i=0; i<256; i++) src[i] = i;
	fb->ConvertString(src, dest, 256);

	//////////////////////////////////////////////////////////////////////////
	//   2. Copy data
	//////////////////////////////////////////////////////////////////////////
	TableItem table[256];
	unsigned char* bitmap_buffer[256];
	unsigned char *buf;
	FT_GlyphSlot slot = fb->m_face->glyph;
	FT_Bitmap bitmap;
	int width = 0;
	word *image=NULL;
	int text_length=1;
	long bitmap_length; 

	for (i=0; i<256; i++)
	{
		FT_Load_Char(fb->m_face, dest[i], FT_LOAD_RENDER);
		bitmap = slot->bitmap;

		table[i].slot_bitmap_left = slot->bitmap_left;
		table[i].slot_bitmap_top = slot->bitmap_top;
		table[i].slot_advance_x = slot->advance.x;
		table[i].slot_advance_y = slot->advance.y;
		table[i].bitmap_width = bitmap.width;
		table[i].bitmap_rows = bitmap.rows;

		bitmap_length = sizeof(unsigned char)*bitmap.width*bitmap.rows;
		bitmap_buffer[i] = (unsigned char*)malloc(bitmap_length);
		if (bitmap_buffer[i]==NULL)
		{
			printf("Memory allocation error: %d bytes\n", bitmap_length);
			delete fb;
			return err;
		}

		if (bitmap.pixel_mode==FT_PIXEL_MODE_GRAY)
		{
			memcpy(bitmap_buffer[i], bitmap.buffer, bitmap_length);
		}		 
		else
		{
			int q=0, x, y, bit;
			unsigned char *image=bitmap_buffer[i];

			for (y=0; y<bitmap.rows; y++)
			{
				buf=bitmap.buffer+q;
				for (x=0; x<bitmap.width; x++)
				{
					bit = (GET_BIT(buf, x))==1 ? 255 : 0;
					image[y*bitmap.width+x]=bit;
				}
				q+=bitmap.pitch;
			}
		}

		if (i==0)
		{
#ifdef VER1 
			table[i].offset= 256*sizeof(TableItem) + sizeof(long);
#else
			table[i].offset= 256*sizeof(TableItem) + sizeof(long)*2;
#endif
		}
		else
		{
			table[i].offset = table[i-1].offset + sizeof(unsigned char)*table[i-1].bitmap_width*table[i-1].bitmap_rows;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//   3. Save data
	//////////////////////////////////////////////////////////////////////////
	FILE* ready = fopen(outputName, "wb");
	if (ready==NULL)
	{
		printf("Cannot open file: %s\n", outputName);
		delete fb;
		return -1;
	}

	height = max(height, new_height);
	long hh=htonl(height);
	fwrite(&hh, 4, 1, ready);

	hh=htonl(baseline);
#ifdef VER1 
	;
#else
	fwrite(&hh, 4, 1, ready);
#endif

	for (i=0; i<256; i++)
	{
		table[i].slot_advance_x = htonl(table[i].slot_advance_x);
		table[i].slot_advance_y	= htonl(table[i].slot_advance_y);
		table[i].slot_bitmap_left =	htonl(table[i].slot_bitmap_left);
		table[i].slot_bitmap_top = htonl(table[i].slot_bitmap_top);
		table[i].bitmap_rows = htonl(table[i].bitmap_rows);
		table[i].bitmap_width = htonl(table[i].bitmap_width);
		table[i].offset	= htonl(table[i].offset);
		fwrite(&(table[i]), sizeof(TableItem), 1, ready);
	}

	for (i=0; i<256; i++)
	{					   
		long w = htonl(table[i].bitmap_width);
		long r = htonl(table[i].bitmap_rows);
		fwrite(bitmap_buffer[i], sizeof(unsigned char), w*r, ready);
	}
	fclose(ready);
	delete fb;

	printf("Finished.\n");

//#ifdef TEST_LIB_RENDER
//	FONT font0;
//	word rgn=0;
//	int err1 = Load_Font(&font0, outputName);
//	if (err1!=0) return 0;
//	TAP_Osd_PutS_Font(rgn, 50, 100,  199, "qUIck", RGB(255,255,255), RGB(0,0,0), &font0, ALIGN_LEFT);
//	Delete_Font(&font0);
//#endif

	return 0;
}


