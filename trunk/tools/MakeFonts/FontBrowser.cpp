#include "fontbrowser.h"
#include "stdio.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b)) 

FontBrowser::FontBrowser()
{
	init_err = FT_Init_FreeType( &m_library );
	if (init_err==0)
	{
		m_face=NULL;
		m_renderMode=FT_RENDER_MODE_MONO;
		m_foreColor= RGB(255, 255, 255);
		m_backColor= RGB(0,0,0);

		m_charmap=NULL;
		m_antialiasmap=NULL;
	}
}

FontBrowser::~FontBrowser()
{
	if (m_face!=NULL)
	{
		FT_Done_Face(m_face);
	}

	if (m_library!=NULL)
	{
		FT_Done_FreeType(m_library);
	}

	if (m_charmap!=NULL)
	{
		free(m_charmap);
	}

	if (m_antialiasmap!=NULL)
	{
		free(m_antialiasmap);
	}
}

int FontBrowser::SetFontName(char* fontName)
{
	int error;


	if (m_library!=NULL)
	{
		if (m_face!=NULL)
		{
		  FT_Done_Face(m_face);
		}
		error = FT_New_Face(m_library, fontName, 0, &m_face); 
		if ( error == FT_Err_Unknown_File_Format ) 
		{ 
			//... the font file could be opened and read, but it appears ... that its font format is unsupported 
			return -1;
		} 
		else 
			if ( error ) 
			{ 
				//... another error code means that the font file could not ... be opened or read, or simply that it is broken... 
				return -2;
			}
	}
	else
	{
		return -3;
	}
	return 0;
}

//face->num_fixed_sizes==0 <- Truetype font
int FontBrowser::SetFontHeight(int* height)
{
	FT_Int i, newsize;
	FT_Error error;


	if (m_library==NULL || m_face==NULL)
	{
		return -1;
	}	

	//error = FT_Set_Char_Size( m_face, 0, (*height) * 64, 0, 0 );
	error = FT_Set_Pixel_Sizes( m_face, 0, (*height) );
	if (error!=0) // sizes do not match
	{
		i = m_face->num_fixed_sizes-1;
		newsize = (m_face->available_sizes[i].y_ppem + 32 ) >> 6;
		FT_Set_Pixel_Sizes(m_face, 0, newsize);

		(*height) = m_face->available_sizes[i].height;
		return 0;
	}
	return 0;
}

void FontBrowser::SetRenderMode(int mode)
{
	if(mode>=FT_RENDER_MODE_MAX) mode = 0;

	m_renderMode = (FT_Render_Mode)mode;
}

int FontBrowser::SetCharMap(char* CharMapName)
{
	FILE *f;
	unsigned int src, dest;
	char line[256], *p1;


	f = fopen(CharMapName, "r");
	if (f==0) return -1;

	if (m_charmap!=NULL)free(m_charmap);
	m_charmap = NULL;

	m_charmap = (unsigned int*)malloc(256*sizeof(unsigned int));
	if (m_charmap==NULL) return -2;

	memset(m_charmap, 0, 256*sizeof(unsigned int));

	while(!feof(f))
	{
		if( fgets( line, 256, f ) != NULL)
		{
			//=<HEX-source>\tU+<hex-destination>\t<Description>
			//=69	U+0069	LATIN SMALL LETTER I

			p1=line;
			while(p1[0]!=0)
			{
				if (p1[0]=='=')
				{
					sscanf(++p1, "%x", &src);
				}

				if (p1[0]=='+')
				{
					sscanf(++p1, "%x", &dest);
					break;
				}
				p1++;
			}
			m_charmap[src]=dest;
		}
	}
	fclose(f);
	return 0;
}

void FontBrowser::SetColors(word foreColor, word backColor)
{
	m_foreColor=foreColor;
	m_backColor=backColor;
	ComputeAntialiasingMap(foreColor, backColor);
}

int FontBrowser::CalcSize(unsigned char* text, int* width, int* height, int *text_length)
{
	int             pen_x=0, pen_y=0, i, t_height, int_adv, ret, tmp_len, tmp_height;
	FT_Error		error;
	FT_GlyphSlot    slot;
	unsigned int    *c_text=NULL;
	FT_Bitmap		bitmap;


	if (m_library==NULL || m_face==NULL)
	{
		return -1;
	}	

	tmp_len = (int)strlen((char*)text);
	c_text = (unsigned int*)malloc(tmp_len*sizeof(unsigned int));
	if (c_text==NULL)
	{
		return -3;
	}
	ret = ConvertString(text, c_text, tmp_len);

	// First calculating resulting bitmap size & cashing bitmaps
	slot = m_face->glyph;

	m_vert_offset = (*height);
	t_height = (*height);

	(*text_length)  = (int)strlen((char*)text);
	for (i=0; i<(*text_length); i++)
	{
		error = FT_Load_Char(m_face, c_text[i], FT_LOAD_RENDER);
		//int_adv = slot->advance.x;
		//int_adv = slot->bitmap.width;
		int_adv = max(slot->advance.x, slot->bitmap.width);

		if ((*width)>0)
		{
		  if ((pen_x + int_adv)>(*width))
		  {
			(*text_length) = i;
			break;
		  }
		}
		bitmap = slot->bitmap;
		pen_x += int_adv;
		tmp_height = max((*height), bitmap.rows);
		t_height = max(t_height, tmp_height);

		m_vert_offset = min(t_height-(bitmap.rows-slot->bitmap_top), m_vert_offset);
	}
	free(c_text);

	(*width) = pen_x>>6;
	(*width) = ((*width)/2+1)*2;
	(*height) = t_height;

	return 0;
}

int FontBrowser::CalcSizeA(char* fontName, unsigned char* text, int* width, int* height, int* text_length)
{
	FT_Error		error;


	error = SetFontName(fontName);
	if (error != 0) return error;

	error = SetFontHeight(height);
	if (error != 0) return error;

	CalcSize(text, width, height, text_length);

	return 0;
}

int FontBrowser::DisplayString(unsigned char* text, int text_length, int* width, int* height, word** image)
{
	FT_GlyphSlot    slot;
	int             pen_x=0, pen_y=0, i, ret;
	int             y_offset, x_offset;
	FT_Vector       origin; 
	FT_Glyph        glyph;
	FT_BitmapGlyph  bitmap_glyph;
	FT_UInt			glyph_index;
	FT_Error		error;
	FT_Bitmap		bitmap;
	word			*img;
	unsigned int    *c_text=NULL;


	if (m_library==NULL || m_face==NULL)
	{
		return -1;
	}	

	slot = m_face->glyph;

	// Create bitmap data
	img = (word*)malloc((*width)*(*height)*sizeof(word));
	if (img==NULL)
	{
		return -3;
	}
	FillImage(img, (*width), (*height));

	c_text = (unsigned int*)malloc(text_length*sizeof(unsigned int));
	if (c_text==NULL)
	{
		return -3;
	}
	ret = ConvertString(text, c_text, text_length);

	pen_x=0; 
	pen_y=m_vert_offset;
	for (i=0; i<text_length; i++)
	{	
		if (m_renderMode==FT_RENDER_MODE_NORMAL)
		{
			error = FT_Load_Char(m_face, c_text[i], FT_LOAD_RENDER);
			if (i==0)	  ///TODO: PROVERIT!!!
			{
				pen_x=-slot->bitmap_left;
			}
			x_offset = pen_x+slot->bitmap_left;
			y_offset = pen_y-slot->bitmap_top;
			bitmap = slot->bitmap;
		}
		else
		{
			glyph_index = FT_Get_Char_Index(m_face, c_text[i]);
			error = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT); 
			if (!error )
			{
				error = FT_Get_Glyph(m_face->glyph, &glyph); 
				if (!error)
				{
					origin.x = 0;
					origin.y = 0; 
					error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, &origin, 1);
					if (!error)
					{
						bitmap_glyph = (FT_BitmapGlyph)glyph;
						x_offset = pen_x+bitmap_glyph->left;  //<0 ? 0 : pen_x+bitmap->left;
						y_offset = pen_y-bitmap_glyph->top;   //<0 ? 0 : pen_y-bitmap->top;
						bitmap = bitmap_glyph->bitmap;
					}
				}
			}
		}
		if (!error)
		{
			draw_bitmap(&bitmap, img, (*width), (*height), x_offset, y_offset);
		}
		pen_x += slot->advance.x >> 6; // increment pen position
		pen_y += slot->advance.y >> 6; // increment pen position
	}
	free(c_text);

	(*image) = img;
	return 1;
}

int FontBrowser::DisplayStringA(char* fontName, unsigned char* text, int* width, int* height, word** image)
{
	int				text_length;
	FT_Error		error;


	error = SetFontName(fontName);
	if (error != 0) return error;
	
	error = SetFontHeight(height);
	if (error != 0) return error;
	
	CalcSize(text, width, height, &text_length);

	return DisplayString(text, text_length, width, height, image);
}

void FontBrowser::draw_bitmap(FT_Bitmap* bitmap, word *image, int width, int height, FT_Int x_offset, FT_Int y_offset)
{
	FT_Int  i, j, q, q1, pos, mask;
	FT_Int  bx, by;
	unsigned char pixel;
	int	bit;
	word C;
	//FILE* f = fopen("C:\\Temp\\trace.txt", "wt");


	if (bitmap->pixel_mode==FT_PIXEL_MODE_GRAY)
	{
		q=0;
		for (by = 0; by<bitmap->rows; by++)
		{
			j = by + y_offset;
			for (bx = 0; bx<bitmap->width; bx++)
			{
				i = bx + x_offset;
				if (i>=0 && j>=0 && i<width && j<height)
				{
					pixel = bitmap->buffer[q];
					if (m_antialiasmap!=NULL)
					{
						C = m_antialiasmap[pixel/16];
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
	}
	else 
	if (bitmap->pixel_mode==FT_PIXEL_MODE_MONO)
	{
		by=0;
		q=0;
		while(by<bitmap->rows)
		{
			bx=0;
			j = by + y_offset;
			q1=q;
			while (bx<bitmap->width)
			{
				pixel = bitmap->buffer[q1];

				for (pos=7; pos>=0; pos--)
				{
				  mask = 1<<pos;
				  bit = pixel & mask;
				  if (bit>0)
				  {
					  i = bx+x_offset+(7-pos);
					  if (i>=0 && j>=0 && i<width && j<height)
					  {
						  PUTPIXEL(i, j, m_foreColor);
					  }
					  //fprintf(f, "0");
				  }
				  else
				  {
					  //fprintf(f, ".");
				  }
				}

				bx+=8;
				q1++;
			}
			//fprintf(f, "\n");
			q+=bitmap->pitch;
			by++;
		}
	}
	//fclose(f);
}

void FontBrowser::FillImage(word *image, int width, int height)
{
	int i;

	for (i=0; i<width*height; i++) image[i] = m_backColor;
}

int FontBrowser::ConvertString(unsigned char* src, unsigned int* dest, int text_length)
{
	int i,l;

	if (text_length==0)	l=(int)strlen((char*)src);
	else l = text_length;

	if (m_charmap!=NULL)
	{
		for (i=0; i<l; i++)
		{
			dest[i] = m_charmap[src[i]];
		}
	}
	else
	{
		for (i=0; i<l; i++)
			dest[i] = (unsigned int)src[i];
	}
	
	return 0; 
}

// computes 16 anti-aliasing steps trough RGB space
void FontBrowser::ComputeAntialiasingMap(word foreColor, word backColor)
{
	int dimmer=16, i;
	int fr, fg, fb, br, bg, bb;


	if (m_antialiasmap!=NULL)
	{
		free(m_antialiasmap);
		m_antialiasmap = NULL;
	}

	if (foreColor!=backColor)
	{
		m_antialiasmap = (word*)malloc(16*sizeof(word));
		fr = GETR(foreColor); fg = GETG(foreColor);  fb = GETB(foreColor);
		br = GETR(backColor); bg = GETG(backColor);  bb = GETB(backColor);

		for(i=0; i<16; i++)
		{
			m_antialiasmap[i] = RGB( (fr*dimmer+(255-dimmer)*br)>>8, (fg*dimmer+(255-dimmer)*bg)>>8, (fb*dimmer+(255-dimmer)*bb)>>8);
			dimmer += 16;
		}
		m_antialiasmap[0] = backColor;
		m_antialiasmap[15] = foreColor;
	}
}


