#include "rasterfontbrowser.h"
#include <string.h>
#include <stdio.h>


#ifdef _WIN32
#include <stdlib.h>
#define _MALLOC				malloc
#define _FREE				free
#define _FILE				FILE
#define _FOPEN(A)			fopen((A), "rb")
#define _FCLOSE				fclose
#define _FREAD				fread
#else
#define _MALLOC				TAP_MemAlloc
#define _FREE				TAP_MemFree
#define _FILE				TYPE_File
#define _FOPEN				TAP_Hdd_Fopen
#define _FCLOSE				TAP_Hdd_Fclose
#define _FREAD				TAP_Hdd_Fread
#endif


extern word rgn;


int Load_Font(FONT *font, char *fileName)
{
	int        ret=-1, read, err;
	_FILE	  *f;
	long       flen;

	
	f = _FOPEN(fileName);
	if (f!=NULL)
	{

#ifndef _WIN32
		flen = TAP_Hdd_Flen(f);
#else
		fseek(f, 0, SEEK_END);
		flen = ftell(f);
		fseek(f, 0, SEEK_SET);
#endif
		font->m_data = (unsigned char*)_MALLOC(flen);
		if (font->m_data!=NULL)
		{
			read = _FREAD(font->m_data, flen, 1, f);
			_FCLOSE(f);
			if (read==1)
			{
				err = Load_Font_Height(font);
				if (err==0)
				{
					err = Load_Char_Table(font);
					if (err==0)
					{
						font->m_antialiasmap=NULL;
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
		if (font->m_antialiasmap != NULL)
		{
			_FREE(font->m_antialiasmap);
		}
		if (font->m_bmpHeaderArray != NULL)
		{
			_FREE(font->m_bmpHeaderArray);
		}
		_FREE(font->m_data);
	}
}


int Load_Font_Height(FONT *font)
{
	int error=1;

	if (font!=NULL)
	{
		font->m_fontHeight = read_long(font, 0);
		error=0;
	}

	return error;
}

int Load_Char_Table(FONT* font)
{
	int error=0;
	int i;
	long offset=4;


	if (font!=NULL)
	{
		font->m_bmpHeaderArray = (TableItem*)_MALLOC(sizeof(TableItem)*256);
		if (font->m_bmpHeaderArray!=NULL)
		{
			for (i=0; i<256; i++)
			{
				font->m_bmpHeaderArray[i].slot_advance_x = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].slot_advance_y = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].slot_bitmap_left = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].slot_bitmap_top = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].bitmap_rows = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].bitmap_width = read_long(font, offset); offset+=4;
				font->m_bmpHeaderArray[i].offset = read_long(font, offset); offset+=4;
			}
		}
		else
		{
			error = 2;
		}
	}
	else
	{
		error = 1;
	}

	return error;
}

int Load_Char(FONT* font, int pos, unsigned char **bitmap)
{
	int error=1;
	int read=0, buffer_length;


	if (font!=NULL && font->m_bmpHeaderArray!=NULL)
	{
		buffer_length = sizeof(unsigned char) * font->m_bmpHeaderArray[pos].bitmap_rows * font->m_bmpHeaderArray[pos].bitmap_width;
		(*bitmap) = font->m_data + font->m_bmpHeaderArray[pos].offset;

		error = 0;
	}

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


	if (font->m_bmpHeaderArray==NULL)
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
	int             pen_x=0, pen_y=0, i, error;
	int             y_offset, x_offset;
	word			*img;
	unsigned char	*bitmap;


	if (font==NULL || font->m_bmpHeaderArray==NULL)
	{
		return -1;
	}	

	// Create bitmap data
	img = (word*)_MALLOC((*width) * font->m_fontHeight * sizeof(word));
	if (img==NULL)
	{
		return -3;
	}
	FillImage(font, img, (*width), font->m_fontHeight);

	pen_x = 0; 
	pen_y = font->m_fontHeight - font->m_vert_offset;
	for (i=0; i<text_length; i++)
	{	
		x_offset = pen_x+font->m_bmpHeaderArray[text[i]].slot_bitmap_left;
		y_offset = pen_y-font->m_bmpHeaderArray[text[i]].slot_bitmap_top;

		error = Load_Char(font, text[i], &bitmap);
		if (!error)
		{
			draw_bitmap(font, bitmap, img, (*width), font->m_fontHeight, x_offset, y_offset, font->m_bmpHeaderArray[text[i]].bitmap_rows, font->m_bmpHeaderArray[text[i]].bitmap_width);
		}
		pen_x += font->m_bmpHeaderArray[text[i]].slot_advance_x >> 6; // increment pen position
		pen_y += font->m_bmpHeaderArray[text[i]].slot_advance_y >> 6; // increment pen position
		//TAP_MemFree(bitmap);
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


	if (font->m_antialiasmap!=NULL)
	{
		_FREE(font->m_antialiasmap);
		font->m_antialiasmap = NULL;
	}

	if (foreColor!=backColor)
	{
		font->m_antialiasmap = (word*)_MALLOC(16*sizeof(word));
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
}


long read_long(FONT *font, long offset)
{
	unsigned char *data_ptr;
	long ret=-1;


	data_ptr = font->m_data+offset;
	ret = ((long)data_ptr[3]<<24) + ((long)data_ptr[2]<<16) + ((long)data_ptr[1]<<8) + ((long)data_ptr[0]);

	return ret;
}


