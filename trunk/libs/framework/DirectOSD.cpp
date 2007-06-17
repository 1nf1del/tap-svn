#include <tap.h>
#include "directosd.h"
#include "rect.h"


word* DirectOSD::m_lineAddr[MAX_SCREEN_Y];

void DirectOSD::Construct()
{
	TYPE_OsdBaseInfo osdBaseInfo;
	TAP_Osd_GetBaseInfo( &osdBaseInfo );

	// set vertical address
	for( int i=0; i < MAX_SCREEN_Y; i+=2 )
	{
		m_lineAddr[i] = (word*)osdBaseInfo.eAddr + 360*i;
		m_lineAddr[i+1] = (word*)osdBaseInfo.oAddr + 360*i;
	}
}


// Draw a line. Only use this to draw lines that are not horizontal or vertical
void DirectOSD::DrawLine(int x1, int y1, int x2, int y2, word color)
{
	int  x, y;
	int  dx, dy;
	int  incx, incy;
	int  balance;

	if (x2 >= x1)
	{
		dx = x2 - x1;
		incx = 1;
	}
	else
	{
		dx = x1 - x2;
		incx = -1;
	}

	if (y2 >= y1)
	{
		dy = y2 - y1;
		incy = 1;
	}
	else
	{
		dy = y1 - y2;
		incy = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy)
	{
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (x != x2)
		{
			SetPixel(x, y,color);
			if (balance >= 0)
			{
				y += incy;
				balance -= dx;
			}
			balance += dy;
			x += incx;
		} 
		SetPixel(x, y,color);
	}
	else
	{
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (y != y2)
		{
			SetPixel(x, y,color);
			if (balance >= 0)
			{
				x += incx;
				balance -= dy;
			}
			balance += dx;
			y += incy;
		}
		SetPixel(x, y,color);
	}
}


// Draw a rectangle, border only
void DirectOSD::DrawRect(int x, int y, int width, int height, int thickness, word color)
{
	FillRect(x,y,							width, thickness, color);					// Top
	FillRect(x,y+height-thickness,			width, thickness, color);					// Bottom
	FillRect(x,y+thickness,					thickness, height-thickness, color);		// Left
	FillRect(x+width-thickness,y+thickness,	thickness, height-thickness, color);		// Right
}


void DirectOSD::Copy(int srcX, int srcY, int width, int height, int destX, int destY)
{
	int x2 = srcX+width-1;
	int y2 = srcY+height;
	int j;
	for (; srcY < y2; ++srcY, ++destY)
	{
		word* src = m_lineAddr[srcY]+srcX;
		word* dest = m_lineAddr[destY]+destX;
		j = srcX;
		for (; j < x2; j+=1)
			*dest = *src;
	}
}


// Draw a filled rectangle, interior only
void DirectOSD::FillRect(int x, int y, int width, int height, word color)
{
	dword dw = color | (color << 16);
	int x2 = x+width-1;
	int y2 = y+height;
	int i,j;
	for (i = y; i < y2; i++)
	{
		word* pw = m_lineAddr[i]+x;
		j = x;
		if (x & 1)
		{
			*pw = color;
			++j;
			++pw;
		}
		dword* pd = (dword*)pw;
		for (; j < x2; j+=2, ++pd)
			*pd = dw;
		if ((x2+1) & 1)
			*(word*)pd = color;
	}
}


// Draw a filled rectangle with a border
void DirectOSD::FrameRect(int x1, int y1, int width, int height, int borderThickness, word fillColor, word borderColor)
{
	DrawRect(x1,y1, width,height, borderThickness, borderColor);
	FillRect(x1+borderThickness,y1+borderThickness, width-borderThickness*2,height-borderThickness*2, fillColor);
}


// Draw a filled rounded rectangle with a border
void DirectOSD::FrameRoundRect(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor)
{
	DrawCorner( x,y, 0, fillColor, borderColor );													// Top left corner
	FillRect( x+radius,y,		width-radius*2,borderThickness,	borderColor);						// Top border
	FillRect( x+radius,y+borderThickness,	width-radius*2,radius-borderThickness,	 fillColor);	// Top fill
	DrawCorner( x+width-radius,y, 1, fillColor, borderColor );										// Top right corner

	FillRect( x,y+radius,		borderThickness,height-radius*2,	borderColor);					// Left border
	FillRect( x+borderThickness,y+radius,	width-borderThickness*2,height-radius*2, fillColor);	// Fill
	FillRect( x+width-borderThickness,y+radius,	borderThickness,height-radius*2,	 borderColor);	// Right border

	DrawCorner( x,y+height-radius, 3, fillColor, borderColor );										// Bottom left corner
	FillRect( x+radius,y+height-radius,	width-radius*2,radius-borderThickness,	 fillColor);		// Bottom border
	FillRect( x+radius,y+height-borderThickness,	width-radius*2,borderThickness,	 borderColor);	// Bottom fill
	DrawCorner( x+width-radius,y+height-radius, 2, fillColor, borderColor );						// Bottom right corner
}


// Draw a rounded rectangle
void DirectOSD::RoundRectTop(int x, int y, int width, int height, int radius, int borderThickness, word fillColor)
{
	DrawCorner( x,y, 0, fillColor );																// Top left corner
	FillRect( x+radius,y+borderThickness,	width-radius*2,radius-borderThickness,	 fillColor);	// Top fill
	DrawCorner( x+width-radius,y, 1, fillColor );													// Top right corner

	FillRect( x+borderThickness,y+radius,	width-borderThickness*2,height-radius, fillColor);		// Fill
}


// Draw a filled rounded rectangle with a border
// /--------\
// |xxxxxxxx|
void DirectOSD::FrameRoundRectTop(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor)
{
	DrawCorner( x,y, 0, fillColor, borderColor );													// Top left corner
	FillRect( x+radius,y,		width-radius*2,borderThickness,	borderColor);						// Top border
	FillRect( x+radius,y+borderThickness,	width-radius*2,radius-borderThickness,	 fillColor);	// Top fill
	DrawCorner( x+width-radius,y, 1, fillColor, borderColor );										// Top right corner

	FillRect( x,y+radius,		borderThickness,height-radius,	borderColor);						// Left border
	FillRect( x+borderThickness,y+radius,	width-borderThickness*2,height-radius, fillColor);		// Fill
	FillRect( x+width-borderThickness,y+radius,	borderThickness,height-radius,		 borderColor);	// Right border
}


// Draw a rounded rectangle
void DirectOSD::RoundRectBottom(int x, int y, int width, int height, int radius, int borderThickness, word fillColor)
{
	FillRect( x+borderThickness,y,			width-borderThickness*2,height-radius,	fillColor);		// Fill

	DrawCorner( x,y+height-radius, 3, fillColor );													// Bottom left corner
	FillRect( x+radius,y+height-radius,	width-radius*2,radius-borderThickness,	 fillColor);		// Bottom fill
	DrawCorner( x+width-radius,y+height-radius, 2, fillColor);										// Bottom right corner
}

// Draw a filled rounded rectangle with a border
// |xxxxxxxx|
// \________/
void DirectOSD::FrameRoundRectBottom(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor)
{
	FillRect( x,y,							borderThickness,height-radius,			borderColor);	// Left border
	FillRect( x+borderThickness,y,			width-borderThickness*2,height-radius,	fillColor);		// Fill
	FillRect( x+width-borderThickness,y,	borderThickness,height-radius,			 borderColor);	// Right border

	DrawCorner( x,y+height-radius, 3, fillColor, borderColor );										// Bottom left corner
	FillRect( x+radius,y+height-radius,	width-radius*2,radius-borderThickness,	 fillColor);		// Bottom fill
	FillRect( x+radius,y+height-borderThickness,	width-radius*2,borderThickness,	 borderColor);	// Bottom border
	DrawCorner( x+width-radius,y+height-radius, 2, fillColor, borderColor );						// Bottom right corner
}


void DirectOSD::FrameRoundRectShadow(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor, word outerColor)
{
	// draw the shadow first
	const int offset = 10;
	DrawCorner( x+width+offset/2-radius,y+offset, 1, outerColor, outerColor );						// Top right corner
	FillRect( x+width,y+offset+radius, offset/2,height-radius*2, outerColor);						// Right fill

	FillRect( x+width+offset/2-radius,y+height, radius,offset-radius, outerColor);
	DrawCorner( x+width+offset/2-radius,y+height+offset-radius, 2, outerColor, outerColor );		// Bottom right corner
	FillRect( x+offset+radius,y+height, width-radius*2-offset/2,offset, outerColor);				// Bottom fill
	DrawCorner( x+offset,y+height+offset-radius, 3, outerColor, outerColor );						// Bottom left corner
	FillRect( x+offset,y+height, radius,offset-radius, outerColor);									// Left fill

	// Now draw the frame and fill
	FrameRoundRectTop(x,y, width,height-radius+1, radius, borderThickness, fillColor, borderColor);

	// Draw the bottom, with the right corner outer drawn in the shadow colour
	DrawCorner( x,y+height-radius, 3, fillColor, borderColor );										// Bottom left corner
	FillRect( x+radius,y+height-radius,	width-radius*2,radius-borderThickness,	 fillColor);		// Bottom border
	FillRect( x+radius,y+height-borderThickness,	width-radius*2,borderThickness,	 borderColor);	// Bottom fill
	DrawCorner( x+width-radius,y+height-radius, 2, fillColor, borderColor, outerColor );			// Bottom right corner
}


byte cornerTLBR[] =
{
	0,0,0,0,0,0,1,1,
	0,0,0,0,1,1,1,1,
	0,0,0,1,1,1,1,2,
	0,0,1,1,1,2,2,2,
	0,1,1,1,2,2,2,2,
	0,1,1,2,2,2,2,2,
	1,1,1,2,2,2,2,2,
	1,1,2,2,2,2,2,2
};
byte cornerTRBL[] =
{
	1,1,0,0,0,0,0,0,
	1,1,1,1,0,0,0,0,
	2,1,1,1,1,0,0,0,
	2,2,2,1,1,1,0,0,
	2,2,2,2,1,1,1,0,
	2,2,2,2,2,1,1,0,
	2,2,2,2,2,1,1,1,
	2,2,2,2,2,2,1,1	
};


void DirectOSD::DrawCorner(int x, int y, int n, word fillColor)
{
	int i,j;
	int dir;
	byte* c = n & 1 ? cornerTRBL : cornerTLBR;
	if ( n >= 2 )
	{
		c += 63;
		dir = -1;
	}
	else
		dir = 1;
	for (i = y; i < y+8; ++i)
	{
		for (j = x; j < x+8; ++j, c+=dir)
		{
			byte cv = *c;
			if ( cv>=2 )
				SetPixel(j,i, fillColor);
		}
	}
}


void DirectOSD::DrawCorner(int x, int y, int n, word fillColor, word borderColor)
{
	int i,j;
	int dir;
	byte* c = n & 1 ? cornerTRBL : cornerTLBR;
	if ( n >= 2 )
	{
		c += 63;
		dir = -1;
	}
	else
		dir = 1;
	for (i = y; i < y+8; ++i)
	{
		for (j = x; j < x+8; ++j, c+=dir)
		{
			byte cv = *c;
			if ( cv )
				SetPixel(j,i, cv>=2 ? fillColor : borderColor);
		}
	}
}


void DirectOSD::DrawCorner(int x, int y, int n, word fillColor, word borderColor, word outerColor)
{
	int i,j;
	int dir;
	byte* c = n & 1 ? cornerTRBL : cornerTLBR;
	if ( n >= 2 )
	{
		c += 63;
		dir = -1;
	}
	else
		dir = 1;
	for (i = y; i < y+8; ++i)
	{
		for (j = x; j < x+8; ++j, c+=dir)
		{
			byte cv = *c;
			SetPixel(j,i, cv ? (cv>=2 ? fillColor : borderColor) : outerColor);
		}
	}
}





// Draw a filled rectangle, interior only
void DirectOSD::FillRect(int x, int y, int width, int height, word color, word increment)
{
	word r, g, b;
	r = (color >> 7) & 0xf8;
	g = (color >> 2) & 0xf8;
	b = (color << 3) & 0xf8;

	word prominent_color;
	if ((r > g && r > b) || (r > g && r == b) || (r > b && r == g) || (r == g && r == b))
		prominent_color = r;
	else if ((g > r && g > b) || (g > r && g == b))
		prominent_color = g;
	else
		prominent_color = b;

	int x2 = x+width-1;
	int y2 = y+height;
	int i,j;
	for (i = y; i < y2; i++)
	{
		color = RGB8888(r,g,b);
		dword dw = color | (color << 16);
		word* pw = m_lineAddr[i]+x;
		j = x;
		if (x & 1)
		{
			*pw = color;
			++j;
			++pw;
		}
		dword* pd = (dword*)pw;
		for (; j < x2; j+=2, ++pd)
			*pd = dw;
		if ((x2+1) & 1)
			*(word*)pd = color;

		// adjust the red, green and blue values according the severity of the gradient
		r = prominent_color < 128 ? ((r + increment) > 255 ? 255 : r + increment) : ((r < increment) ? 0 : r - increment);
		g = prominent_color < 128 ? ((g + increment) > 255 ? 255 : g + increment) : ((g < increment) ? 0 : g - increment);
		b = prominent_color < 128 ? ((b + increment) > 255 ? 255 : b + increment) : ((b < increment) ? 0 : b - increment);
	}
}
