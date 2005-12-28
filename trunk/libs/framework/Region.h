/*
	Copyright (C) 2005 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __REGION_H
#define __REGION_H


class Region
{
public:
	Region();
	~Region();

	int	Create( dword x, dword y, dword width, dword height, bool memory = false )
	{
		region = TAP_Osd_Create( x,y, width,height, 0, memory ? OSD_Flag_MemRgn : 0 );
	}
	int	Move( dword x, dword y )
	{
		TAP_Osd_Move( region, x, y );
	}

	void DrawLine( int x1, int y1, int x2, int y2, dword color );
	void DrawBox( dword x, dword y, dword width, dword height, dword thickness, dword color )
	{
		TAP_Osd_DrawRectangle( region, x,y, width,height, thickness, color );
	}
	void FillBox( dword x, dword y, dword width, dword height, dword color )
	{
		TAP_Osd_FillBox( region, x,y, width,height, color );
	}
	void Draw3dBox( dword x, dword y, dword width, dword height, dword color1, dword color2 )
	{
		TAP_Osd_Draw3dBox( region, x,y, width,height, color1, color2 );
	}
	void Fill3dBox( dword x, dword y, dword width, dword height, dword color1, dword color2, dword foreColor )
	{
		TAP_Osd_Draw3dBoxFill( region, x,y, width,height, color1, color2, foreColor );
	}

	int	DrawPixmap( dword x, dword y, dword width, dword height, void *pixmap, bool sprite, byte dataFormat )
	{
		TAP_Osd_DrawPixmap( region, x,y, width,height, pixmap, sprite, dataFormat );
	}
	int	DrawGd( int x, int y, TYPE_GrData* gd, bool sprite )
	{
		TAP_Osd_PutGd( region, x,y, gd, sprite );
	}
	byte* SaveBox( dword x, dword y, dword width, dword height );
	void RestoreBox( dword x, dword y, dword width, dword height, void *data );
	int	PutBox( dword x, dword y, dword width, dword height, const void *data, bool sprite, byte dataFormat );
	int	GetBox( dword x, dword y, dword width, dword height, void *data );

	int Zoom( int xzoom, int yzoom );
	static int Copy( word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword width, dword height, dword dstX, dword dstY, bool sprite );

	int	GetPixel( dword x, dword y, void *pix );
	int	PutPixel( dword x, dword y, dword pix );

	static int GetStringWidth( const char *str, TYPE_FontSize fontSize )
	{
		return TAP_Osd_GetW( str, 0, fontSize );
	}
	int	DrawString( dword x, dword y, dword maxX, const char* str, word foreColor, word backColor, TYPE_FontSize fontSize, bool ellipsis, TYPE_Align align )
	{
		return TAP_Osd_PutS( region, x,y, maxX, str, foreColor, backColor, 0, fontSize, ellipsis, align );
	}
	int	PutString(word rgn, dword x, dword y, dword maxX, const char* str, word foreColor, word backColor, TYPE_FontSize fontSize, bool nextLine )
	{
		return TAP_Osd_PutString( region, x,y, maxX, str, foreColor, backColor, 0, fontSize, nextLine );
	}
	int	PutStringAf(dword x, dword y, dword maxX, const char* str, word foreColor, word backColor, TYPE_FontSize fontSize, bool nextLine )
	{
		return TAP_Osd_PutStringAf( region, x,y, maxX, str, foreColor, backColor, 0, fontSize, 0 );
	}

private:
	word region;
};


#endif
