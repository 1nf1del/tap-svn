/*
Copyright Pavel Kopylov

v. 0.0.2 - 15.06.2007
- All "long" data Endianess is changed
- read_long eliminated
- macro htonl is used to compile under Tapulator environment
- Draw_Font_String function added
- m_antialiasmap is not dynamic anymore

v. 0.0.1 - 11.06.2007
First release
*/

#include "RasterFontBrowser.h"
#include <string.h>
#include <stdio.h>

extern word rgn;

#define htonl(A) (  (((long)(A) & 0xff000000) >> 24) | (((long)(A) & 0x00ff0000) >> 8)  | (((long)(A) & 0x0000ff00) << 8)  | (((long)(A) & 0x000000ff) << 24)    )


int Draw_Font_String(word region, dword x, dword y, unsigned char *text, unsigned char* fontname,  word foreColor, word backColor)
{
	int err1, width=0, text_length;
	FONT font;
	word *image=NULL;


	err1 = Load_Font(&font, fontname);
	if (err1==0)
	{
		SetColors(&font, COLOR_Yellow, COLOR_DarkBlue);
		err1 = CalcSize(&font, text, &width, &text_length);
		if (err1==0)
		{
			err1 = RenderString(&font, text, text_length, &width, &image);
			if (err1==0)
			{
				TAP_Osd_DrawPixmap(rgn, x, y, width, font.m_fontHeight, image, FALSE, OSD_1555);
				TAP_MemFree(image);
			}
		}
		Delete_Font(&font);
	}

	return err1;
}

int Load_Font(FONT *font, char *fileName)
{
	int			ret=-1, read, err;
	TYPE_File	*f;
	long		flen;

	
	f = TAP_Hdd_Fopen(fileName);
	if (f!=0)
	{
		font->m_bmpHeaderArray=NULL;

		flen = TAP_Hdd_Flen(f);
		font->m_data = (unsigned char*)TAP_MemAlloc(flen);
		if (font->m_data!=NULL)
		{
			read = TAP_Hdd_Fread(font->m_data, flen, 1, f);
			TAP_Hdd_Fclose(f);
			if (read==1)
			{
				err = Load_Font_Height(font);
				if (err==0)
				{
					err = Load_Char_Table(font);
					if (err==0)
					{
						ret = 0;
					} 
					else
					{
						//Font table didn't load
						ret = 5;
					}
				}
				else
				{
					//Font height not determined
					ret = 4;
				}
			}
			else
			{
				//File didn't read
				ret = 3;
			}
		}
		else
		{
			// not enough memory
			ret = 2;
		}
	}
	else
	{
		// File didn't open
		ret = 1;
	}

	return ret;
}

void Delete_Font(FONT *font)
{
	if (font != NULL)
	{
		TAP_MemFree(font->m_data);
	}
}

int Load_Font_Height(FONT *font)
{
	int error=1;
	long *ptr;


	if (font!=NULL)
	{
		ptr = (long*)(font->m_data);
		font->m_fontHeight = *ptr;
#ifdef _WIN32
		font->m_fontHeight = htonl(font->m_fontHeight);
#endif
		error=0;
	}

	return error;
}

int Load_Char_Table(FONT* font)
{
	int error=1;
	//char msg[128];
#ifdef _WIN32
	int i;
#endif


    if (font!=NULL)
	{
		font->m_bmpHeaderArray = (TableItem*)(font->m_data+4);
		error=0;
	}

#ifdef _WIN32
	for (i=0; i<256; i++)
	{
		font->m_bmpHeaderArray[i].slot_advance_x = htonl(font->m_bmpHeaderArray[i].slot_advance_x);
		font->m_bmpHeaderArray[i].slot_advance_y = htonl(font->m_bmpHeaderArray[i].slot_advance_y);
		font->m_bmpHeaderArray[i].slot_bitmap_left = htonl(font->m_bmpHeaderArray[i].slot_bitmap_left);
		font->m_bmpHeaderArray[i].slot_bitmap_top = htonl(font->m_bmpHeaderArray[i].slot_bitmap_top);
		font->m_bmpHeaderArray[i].bitmap_rows = htonl(font->m_bmpHeaderArray[i].bitmap_rows);
		font->m_bmpHeaderArray[i].bitmap_width = htonl(font->m_bmpHeaderArray[i].bitmap_width);
		font->m_bmpHeaderArray[i].offset = htonl(font->m_bmpHeaderArray[i].offset);
	}
#endif
	//TAP_SPrint(msg, "[%lu, %lu] [%lu, %lu] [%lu, %lu] %lu", font->m_bmpHeaderArray[43].slot_advance_x,
	//font->m_bmpHeaderArray[43].slot_advance_y,
	//font->m_bmpHeaderArray[43].slot_bitmap_left,
	//font->m_bmpHeaderArray[43].slot_bitmap_top,
	//font->m_bmpHeaderArray[43].bitmap_rows,
	//font->m_bmpHeaderArray[43].bitmap_width,
	//font->m_bmpHeaderArray[43].offset);
	//TAP_Osd_PutString(rgn, 40, 50, 300, msg, COLOR_Yellow, COLOR_DarkBlue, 0, FNT_Size_1419, 0);

	return error;
}


void SetColors(FONT *font, word foreColor, word backColor)
{
	font->m_foreColor=foreColor;
	font->m_backColor=backColor;
	ComputeAntialiasingMap(font, foreColor, backColor);
}

int CalcSize(FONT *font, unsigned char* text, int* width, int *text_length)
{
	int pen_x=0, pen_y=0, i, t_height, int_adv, tmp_len;


	if (font==NULL || font->m_bmpHeaderArray==NULL)
	{
		return -1;
	}	
	tmp_len = (int)strlen(text);

	// First calculating resulting bitmap size & cashing bitmaps
	font->m_vert_offset = font->m_fontHeight;
	t_height = font->m_fontHeight;

	(*text_length)  = (int)strlen(text);
	for (i=0; i<(*text_length); i++)
	{
		int_adv = font->m_bmpHeaderArray[text[i]].slot_advance_x;
		if ((*width)>0)
		{
			if ((pen_x + int_adv)>(*width))
			{
				(*text_length) = i;
				break;
			}
		}
		pen_x += int_adv;
		font->m_vert_offset = font->m_fontHeight - font->m_bmpHeaderArray[text[i]].slot_bitmap_top < font->m_vert_offset ? 
		font->m_fontHeight - font->m_bmpHeaderArray[text[i]].slot_bitmap_top : font->m_vert_offset;
	}

	(*width) = pen_x>>6;
	(*width) = ((*width)/2+1)*2;
	font->m_fontHeight = t_height;

	return 0;
}

int RenderString(FONT *font, unsigned char* text, int text_length, int* width, word** image)
{
	int             pen_x=0, pen_y=0, i;
	int             y_offset, x_offset;
	word			*img;
	unsigned char	*bitmap;


	if (font==NULL || font->m_bmpHeaderArray==NULL)
	{
		return -1;
	}	

	// Create bitmap data
	img = (word*)TAP_MemAlloc((*width) * font->m_fontHeight * sizeof(word));
	if (img==NULL)
	{
		return -2;
	}
	FillImage(font, img, (*width), font->m_fontHeight);

	pen_x = 0; 
	pen_y = font->m_fontHeight - font->m_vert_offset;
	for (i=0; i<text_length; i++)
	{	
		x_offset = pen_x+font->m_bmpHeaderArray[text[i]].slot_bitmap_left;
		y_offset = pen_y-font->m_bmpHeaderArray[text[i]].slot_bitmap_top;

		LOAD_CHAR(font, text[i], &bitmap);
		draw_bitmap(font, bitmap, img, (*width), font->m_fontHeight, x_offset, y_offset, font->m_bmpHeaderArray[text[i]].bitmap_rows, font->m_bmpHeaderArray[text[i]].bitmap_width);

		pen_x += font->m_bmpHeaderArray[text[i]].slot_advance_x >> 6; // increment pen position
		pen_y += font->m_bmpHeaderArray[text[i]].slot_advance_y >> 6; // increment pen position
	}

	(*image) = img;
	return 0;
}

void draw_bitmap(FONT *font, unsigned char* bitmap, word *image, int width, int height, int x_offset, int y_offset, int bitmap_rows, int bitmap_width)
{
	int  i, j, q;
	int  bx, by;
	unsigned char pixel;
	word C;
	//FILE* f = fopen("C:\\Temp\\trace.txt", "wt");


	q=0;
	for (by = 0; by<bitmap_rows; by++)
	{
		j = by + y_offset;
		for (bx = 0; bx<bitmap_width; bx++)
		{
			i = bx + x_offset;
			if (i>=0 && j>=0 && i<width && j<height)
			{
				pixel = bitmap[q];
				if (font->m_antialiasmap!=NULL)
				{
					C = font->m_antialiasmap[pixel/16];
				}
				else
				{
					C=RGB(pixel, pixel, pixel);
				}
				PUTPIXEL(i, j, C);
				//if (pixel==0) {fprintf(f, ".");}
				//else {fprintf(f, "0");}
			}
			q++;
		}
		//fprintf(f, "\n");
	}
	//fclose(f);
}

void FillImage(FONT *font, word *image, int width, int height)
{
	int i;

	for (i=0; i<width*height; i++) image[i] = font->m_backColor;
}

// computes 16 anti-aliasing steps trough RGB space
void ComputeAntialiasingMap(FONT *font, word foreColor, word backColor)
{
	int dimmer=16, i;
	int fr, fg, fb, br, bg, bb;


	if (foreColor!=backColor)
	{
		fr = GETR(foreColor); fg = GETG(foreColor);  fb = GETB(foreColor);
		br = GETR(backColor); bg = GETG(backColor);  bb = GETB(backColor);

		for(i=0; i<16; i++)
		{
			font->m_antialiasmap[i] = RGB( (fr*dimmer+(255-dimmer)*br)>>8, (fg*dimmer+(255-dimmer)*bg)>>8, (fb*dimmer+(255-dimmer)*bb)>>8);
			dimmer += 16;
		}
		font->m_antialiasmap[0] = backColor;
		font->m_antialiasmap[15] = foreColor;
	}
	else
	{
		for(i=0; i<16; i++)
		{
			font->m_antialiasmap[i] = backColor;
		}
	}
}



word* lineAddr[576];

void InitDirect()
{
	int i;
	TYPE_OsdBaseInfo osdBaseInfo;
	TAP_Osd_GetBaseInfo( &osdBaseInfo );

	// set vertical address
	for( i=0; i < 576; i+=2 )
	{
		lineAddr[i] = (word*)osdBaseInfo.eAddr + 360*i;
		lineAddr[i+1] = (word*)osdBaseInfo.oAddr + 360*i;
	}
}


void FillRectDirect(int x, int y, int width, int height, word color)
{
	dword dw = color | (color << 16);
	int x2 = x+width-1;
	int y2 = y+height;
	int i,j;
	for (i = y; i < y2; i++)
	{
		dword* pd;
		word* pw = lineAddr[i]+x;
		j = x;
		if (x & 1)
		{
			*pw = color;
			++j;
			++pw;
		}
		pd = (dword*)pw;
		for (; j < x2; j+=2, ++pd)
			*pd = dw;
		if ((x2+1) & 1)
			*(word*)pd = color;
	}
}


int RenderStringDirect(FONT *font, unsigned char* text, int text_length, int x, int y, int width)
{
	int             pen_x, pen_y, i;
	int             y_offset, x_offset;
	unsigned char	*bitmap;

	if (font==NULL || font->m_bmpHeaderArray==NULL)
	{
		return -1;
	}	

	FillRectDirect(x,y, width, font->m_fontHeight, font->m_backColor);

	pen_x = x; 
	pen_y = y + font->m_fontHeight - font->m_vert_offset;
	for (i=0; i<text_length; i++)
	{
		x_offset = pen_x+font->m_bmpHeaderArray[text[i]].slot_bitmap_left;
		y_offset = pen_y-font->m_bmpHeaderArray[text[i]].slot_bitmap_top;

		// Load_Char
		LOAD_CHAR(font, text[i], &bitmap);
		draw_bitmap_direct(font, bitmap, width, font->m_fontHeight, x_offset, y_offset, font->m_bmpHeaderArray[text[i]].bitmap_rows, font->m_bmpHeaderArray[text[i]].bitmap_width);

		pen_x += font->m_bmpHeaderArray[text[i]].slot_advance_x >> 6; // increment pen position
		pen_y += font->m_bmpHeaderArray[text[i]].slot_advance_y >> 6; // increment pen position
	}

	return 0;
}


void draw_bitmap_direct(FONT *font, unsigned char* bitmap, int width, int height, int x_offset, int y_offset, int bitmap_rows, int bitmap_width)
{
	int  x, y, q;
	int  bx, by;
	unsigned char pixel;
	word c;

	q=0;
	for (by = 0; by<bitmap_rows; ++by)
	{
		y = by + y_offset;
		for (bx = 0; bx<bitmap_width; ++bx)
		{
			x = bx + x_offset;
			if (x>=0 && y>=0 && x<720 && y<576)
			{
				pixel = bitmap[q];
				if (font->m_antialiasmap!=NULL)
				{
					c = font->m_antialiasmap[pixel/16];
				}
				else
				{
					c = RGB(pixel, pixel, pixel);
				}
				*(lineAddr[y]+x) = c;
			}
			++q;
		}
	}
}
