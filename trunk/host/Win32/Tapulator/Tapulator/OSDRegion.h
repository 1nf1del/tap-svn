//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#pragma once
class Window;

#include "InfoLogger.h"
#include "BackingBitmap.h"
#include "GDStore.h"
class Regions;

class OSDRegion : public InfoLogger, public BackingBitmap
{
public:
	OSDRegion( DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag, int index, Regions* pParent, bool bMemRegion );
	~OSDRegion(void);

	int GetIndex();
	void AddWindow(Window* wnd);
	void RemoveWindow(Window* wnd);
	bool AreAnyWindowsUpdated();


	void DrawToBitmap();
	void DrawFilledBox(CRect rect, DWORD dwFillColor, DWORD dwEdgeColor);
	void DrawSomeText(CRect rect, CString txt, DWORD color, DWORD backcolor, int ifont, int iPointSize, DWORD dwFlags, bool bTransparent = false);
	void DrawTextInBox(CRect rect, CString text, DWORD dwForeColor, DWORD dwBackColor, DWORD dwBorderColor, int iFont, int iSize, DWORD dwFlags);
	void DrawRectangle(CRect rect, DWORD dwColor, int thickness);

	int Move(DWORD x, DWORD y);
	int FillBox(DWORD x, DWORD y, DWORD w, DWORD h, DWORD color);
	int PutBox(DWORD x, DWORD y, DWORD w, DWORD h, const void *data, bool sprite, BYTE dataFormat );
	int GetBox(DWORD x, DWORD y, DWORD w, DWORD h, void *data);
	int Ctrl(DWORD ctrl);
	int SetTransparency(char rt);

	int Draw3dBox(DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2 );
	int Draw3dBoxFill(DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2, DWORD fc );

	int DrawRectangle(DWORD x, DWORD y, DWORD w, DWORD h, DWORD t, DWORD color);
	int DrawPixmap(DWORD x, DWORD y, DWORD w, DWORD h, void *pixmap, bool sprite, BYTE dataFormat );
	int Copy(OSDRegion* pSrcRegion, DWORD srcX, DWORD srcY, DWORD w, DWORD h, DWORD dstX, DWORD dstY,  bool sprite);
	int	PutGd(int x, int y, TYPE_GrData * gd, bool sprite);

	BYTE* SaveBox(DWORD x, DWORD y, DWORD w, DWORD h );
	void RestoreBox(DWORD x, DWORD y, DWORD w, DWORD h, void *data );

	int GetPixel(DWORD x, DWORD y, void *pix );
	int PutPixel(DWORD x, DWORD y, DWORD pix );

	int Osd_PutS(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE bDot, BYTE align);
	int Osd_PutString(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine);
	int Osd_PutStringAf(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine);
//	int Osd_DrawString(const char * str, DWORD dstWidth, WORD color, BYTE * dest, DWORD maxWidth, BYTE fntType, BYTE fntSize);
	int GetW(const char *str, BYTE fntType, BYTE fntSize );

private:
	CFont* CreateFont(int iFont, int iFontSize);
	void InvalidateFrameBuffer();
	int GetXOffs();
	int GetYOffs();

	DWORD m_x;
	DWORD m_y;
	DWORD m_w;
	DWORD m_h;
	BYTE m_lutIdx;
	int m_flag;

	int m_Index;
	CDC* GetDC();
	void ReleaseDC();

	std::list<Window*> m_Windows;

	Regions* m_pParent;

	bool m_bMemRegion;

	GDStore m_GDs;
};
