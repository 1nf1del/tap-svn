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

#include "StdAfx.h"
#include ".\osdregion.h"
#include "Window.h"
#include "Regions.h"
#include "Heap.h"
#include "Utils.h"

COLORREF ConvertRGB(DWORD dwToppyColor)
{
	if (dwToppyColor == COLOR_None)
		return TRANSPARENT_COLOUR;

	int r = (dwToppyColor & 0x00007c00) >> 10;
	int g = (dwToppyColor & 0x000003E0) >> 5;
	int b = (dwToppyColor & 0x0000001F);
	return WinRGB(r<<3,g<<3,b<<3);
}

WORD UnConvertRGB(COLORREF rgb)
{
	int r = (rgb & 0x00ff0000) >> 19;
	int g = (rgb & 0x0000ff00) >> 11;
	int b = (rgb & 0x000000ff) >> 3;
	return RGB(r,g,b);
}

OSDRegion::OSDRegion( DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag, int index, Regions* pParent,
					 bool bMemRegion) :
m_x(x), m_y(y), m_w(w), m_h(h), m_lutIdx(lutIdx), m_flag(flag), m_Index(index), m_pParent(pParent),
m_bMemRegion(bMemRegion)
{
}

OSDRegion::~OSDRegion(void)
{
	if (!m_bMemRegion)
	{
		CDC* pDC = GetDC();
		pDC->FillSolidRect(CRect(CPoint(m_x, m_y), CSize(m_w, m_h)), TRANSPARENT_COLOUR);
		ReleaseDC();

	}
}

int OSDRegion::GetIndex()
{
	return m_Index;
}

void OSDRegion::AddWindow(Window* wnd)
{	
	m_Windows.push_back(wnd);
}

void OSDRegion::RemoveWindow(Window* wnd)
{
	m_Windows.remove(wnd);
}

//void OSDRegion::DrawToDC(CDC* pWinDC)
//{
//	CDC* pMemDC = GetDC();
//	DrawToBitmap();
//	pWinDC->BitBlt(m_x, m_y, m_w, m_h, pMemDC, 0, 0, SRCCOPY);
//	ReleaseDC();
//}

void OSDRegion::DrawToBitmap()
{
	std::list<Window*>::iterator it;
	for (it = m_Windows.begin(); it != m_Windows.end(); it++)
	{	
		(*it)->DrawOnRegion();
	}
}

int OSDRegion::Move(DWORD x, DWORD y)
{
	m_x = x;
	m_y = y;
	return 1;
}

int OSDRegion::FillBox(DWORD x, DWORD y, DWORD w, DWORD h, DWORD color)
{
	CRect rect(CPoint(x,y), CSize(w,h));
	rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	DrawFilledBox(rect,color, color);
	return 1;
}

int OSDRegion::PutBox(DWORD x, DWORD y, DWORD w, DWORD h, const void *data, bool sprite, BYTE dataFormat )
{
	if (dataFormat!= OSD_1555)
	{
		LogUnimplemented("PutBox only implemented for 1555 RGB data");
		return 0;
	}

	CDC* pDC = GetDC();

	int dx = x + GetXOffs();
	int dy = y + GetYOffs();

	word* pData = (word*) data;
	for (unsigned int j = 0; j<h; j++)
	{
		for (unsigned int i = 0; i<w; i++)
		{
			if (!sprite || (*pData != 0))
			{
				pDC->SetPixelV(i + dx, j + dy, ConvertRGB(SWAPWORD(*pData)));
			}
			pData++;
		}
	}

	ReleaseDC();
	return 0;
}

int OSDRegion::GetBox(DWORD x, DWORD y, DWORD w, DWORD h, void *data)
{
	CDC* pDC = GetDC();
	CBitmap areaBM;
	areaBM.CreateCompatibleBitmap(pDC, w, h);
	CDC destDC;
	destDC.CreateCompatibleDC(pDC);
	destDC.SelectObject(&areaBM);
	destDC.BitBlt(0, 0, w, h, pDC, x + GetXOffs(), y + GetYOffs(), SRCCOPY);
	ReleaseDC();

	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = w;
	bmInfo.bmiHeader.biHeight = -(int)h;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 16;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	int lines = GetDIBits(destDC, (HBITMAP)areaBM.m_hObject, 0, h, data, &bmInfo, DIB_RGB_COLORS);

	WORD* pixel = (WORD*)data;
	int dataSize = w * lines;
    while ( dataSize-- > 0 )
	{
		*pixel = SWAPWORD(*pixel);
		++pixel;
	}

	return lines != h ? 1 : 0;
}

int OSDRegion::Ctrl(DWORD ctrl)
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int OSDRegion::SetTransparency(char rt)
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}


int OSDRegion::Draw3dBox(DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2 )
{
	CDC* pDC = GetDC();
	pDC->Draw3dRect(x + GetXOffs(), y + GetYOffs(), w, h, ConvertRGB(c1), ConvertRGB(c2));
	ReleaseDC();
	return 0;
}

int OSDRegion::Draw3dBoxFill(DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2, DWORD fc )
{
	CDC* pDC = GetDC();
	pDC->FillSolidRect(x + GetXOffs(), y + GetYOffs(), w, h, ConvertRGB(fc));
	pDC->Draw3dRect(x + GetXOffs(), y + GetYOffs(), w, h, ConvertRGB(c1), ConvertRGB(c2));
	ReleaseDC();
	return 0;
}


int OSDRegion::DrawRectangle(DWORD x, DWORD y, DWORD w, DWORD h, DWORD t, DWORD color)
{
	CRect rect(CPoint(x,y), CSize(w,h));
	rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	DrawRectangle(rect,color,t);
	return 1;
}

int OSDRegion::DrawPixmap(DWORD x, DWORD y, DWORD w, DWORD h, void *pixmap, bool sprite, BYTE dataFormat )
{
	CDC* pDC = GetDC();
	if (dataFormat == OSD_256)
	{
		BYTE* data = (BYTE*) pixmap;
		int lutCount = Regions::GetLutLen();
		DWORD* lut = Regions::GetLut();
		for (unsigned int j = 0; j<h; j++)
		{
			for (unsigned int i = 0; i<w; i++)
			{
				int iIndex = *data;
				data++;
				if ((iIndex == 0) && sprite)
					continue;

				pDC->SetPixelV(i+x + GetXOffs(), y+j + GetYOffs(), lut[iIndex]);
			}
		}
	}
	else if (dataFormat == OSD_1555)
	{
		WORD* pSource = (WORD*) pixmap;
	
		CBitmap bm;
		bm.CreateCompatibleBitmap(pDC, w, h);
		BITMAPINFO bmInfo;
		ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = w;
		bmInfo.bmiHeader.biHeight = -((int)h);
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = 16;
		bmInfo.bmiHeader.biCompression = BI_RGB;

		::SetDIBits(pDC->m_hDC, (HBITMAP)bm.m_hObject, 0, h, pSource, &bmInfo, 0);

		CDC newDC;
		newDC.CreateCompatibleDC(pDC);
		newDC.SelectObject(&bm);
		if ( sprite )
			pDC->TransparentBlt(x + GetXOffs(), y + GetYOffs(), w, h, &newDC, 0, 0, w, h, 0);
		else
			pDC->BitBlt(x  + GetXOffs(), y + GetYOffs(), w, h, &newDC, 0, 0, SRCCOPY);
	}
	else
	{
		LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
		return 1;
	}

	ReleaseDC();
	return 0;
}

int OSDRegion::Copy(OSDRegion* pSrcRegion, DWORD srcX, DWORD srcY, DWORD w, DWORD h, DWORD dstX, DWORD dstY,  bool sprite)
{
	CDC* pDC = GetDC();
	CDC* pSrcDC = pSrcRegion->GetDC();

	if (sprite)
	{
		TransparentBlt(pDC->m_hDC, dstX + GetXOffs(), dstY + GetYOffs(), w, h, pSrcDC->m_hDC, srcX, srcY, w, h, 0);
	}
	else
	{
		pDC->BitBlt(dstX + GetXOffs(), dstY + GetYOffs(), w, h, pSrcDC, srcX, srcY, SRCCOPY);
	}

	ReleaseDC();
	pSrcRegion->ReleaseDC();
	return 0;
}

int	OSDRegion::PutGd(int x, int y, TYPE_GrData * gd, bool sprite)
{
	int w = gd->width;
	int h = gd->height;

	BYTE* pData = m_GDs.GetBitmapDataForGD(gd);

	//CRect rect(CPoint(x,y), CSize(w, h));
	//rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	//DrawFilledBox(rect, COLOR_Blue, COLOR_Cyan);

	DrawPixmap(x, y, w, h, pData, sprite, OSD_1555);

	return 0;
}


BYTE* OSDRegion::SaveBox(DWORD x, DWORD y, DWORD w, DWORD h )
{
	BYTE* data = (BYTE*) Heap::GetTheHeap()->Allocate(w * h * 2);
	GetBox(x, y, w, h, data);
	return data;
}

void OSDRegion::RestoreBox(DWORD x, DWORD y, DWORD w, DWORD h, void *data )
{
	PutBox(x, y, w, h, data, false, OSD_1555);
}


int OSDRegion::GetPixel(DWORD x, DWORD y, void *pix )
{
	word* pPixel = (word*) pix;
	CDC* pDC = GetDC();
	COLORREF rgb = pDC->GetPixel(x + GetXOffs(), y + GetYOffs());
	ReleaseDC();
	*pPixel = UnConvertRGB(rgb);
	return 0;
}

int OSDRegion::PutPixel(DWORD x, DWORD y, DWORD pix )
{
	CDC* pDC = GetDC();
	pDC->SetPixelV(x + GetXOffs(), y + GetYOffs(), ConvertRGB(pix));
	ReleaseDC();
	return 0;
}

int OSDRegion::Osd_PutS(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE bDot, BYTE align)
{
	CRect rect(CPoint(x,y), CSize(maxX+x-1, 1000));
	rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	DWORD dwFlags = DT_TOP;
	switch(align)
	{
	case ALIGN_LEFT:
		dwFlags |= DT_LEFT;
		break;
	case ALIGN_CENTER:
		dwFlags |= DT_CENTER;
		break;
	case ALIGN_RIGHT:
		dwFlags |= DT_RIGHT;
		break;
	default:
		dwFlags |= DT_LEFT;
	}

	DrawSomeText(rect, str, fcolor, bcolor, fntType, fntSize, dwFlags, 0);
	return 1;
}

int OSDRegion::Osd_PutString(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine)
{
	CRect rect(CPoint(x,y), CSize(maxX+x-1, 1000));
	rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	DrawSomeText(rect, str, fcolor, bcolor, fntType, fntSize, DT_LEFT|DT_TOP, bcolor == 0x00);
	return 1;
}

int OSDRegion::Osd_PutStringAf(DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine)
{
	CRect rect(CPoint(x,y), CSize(maxX+x-1, 1000));
	rect.OffsetRect(CPoint(GetXOffs(), GetYOffs()));
	DrawSomeText(rect, str, fcolor, bcolor, fntType, fntSize, DT_LEFT|DT_TOP, bcolor == 0x00);
	return 1;
}


CDC* OSDRegion::GetDC()
{
	if (m_bMemRegion)
		return BackingBitmap::GetDC();

	return m_pParent->GetDC();
}

void OSDRegion::ReleaseDC()
{
	if (m_bMemRegion)
	{
		BackingBitmap::ReleaseDC();
		return;
	}


	m_pParent->ReleaseDC();
}

void OSDRegion::DrawRectangle(CRect rect, DWORD dwColor, int thickness)
{
	CDC* pDC = GetDC();
	COLORREF crBorder = ConvertRGB(dwColor);
	CPen pen(PS_SOLID, thickness, crBorder);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left,rect.top);
	pDC->LineTo(rect.right-1,rect.top);
	pDC->LineTo(rect.right-1,rect.bottom-1);
	pDC->LineTo(rect.left,rect.bottom-1);
	pDC->LineTo(rect.left,rect.top);
	pDC->SelectObject(pOldPen);
	ReleaseDC();
}

void OSDRegion::DrawFilledBox(CRect rect, DWORD dwFillColor, DWORD dwEdgeColor)
{
	CDC* pDC = GetDC();

	COLORREF crBack = ConvertRGB(dwFillColor);
	pDC->FillSolidRect(rect, crBack);
	DrawRectangle(rect, dwEdgeColor, 1);

	ReleaseDC();
}

CFont* OSDRegion::CreateFont(int iFont, int iFontSize)
{
	CDC* pDC = GetDC();
	int iFontPoints = iFontSize == 0 ? -120 : iFontSize == 1 ? -140 : -180;

	CFont* thefont  = new CFont();

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfWeight = FW_BOLD;
	logFont.lfHeight = iFontPoints;
	strcpy(logFont.lfFaceName, "Arial Narrow" );

	thefont->CreatePointFontIndirect(&logFont, pDC);

	ReleaseDC();
	return thefont;
}

void OSDRegion::DrawSomeText(CRect rect, const CString& txt, DWORD color, DWORD backcolor, int font, int iFontSize, 
							 DWORD dwFlags,
							 bool bTransparent)
{
	CDC* pDC = GetDC();

	CFont* theFont = CreateFont(font, iFontSize);
	CFont* oldFont = pDC->SelectObject(theFont);
	
	if (!bTransparent)
	{
		CSize sz = pDC->GetTextExtent(txt);
		DrawFilledBox(CRect(rect.TopLeft(), sz), backcolor, backcolor);
	}

	pDC->SetBkMode(bTransparent ? TRANSPARENT : OPAQUE);
	pDC->SetBkColor(ConvertRGB(backcolor));
	pDC->SetTextColor(ConvertRGB(color));
	CString text = txt;
	text.Replace("&", "&&");
	pDC->DrawText(text, rect, dwFlags);
	pDC->SelectObject(oldFont);

	delete theFont;
	ReleaseDC();
}

void OSDRegion::DrawTextInBox(CRect rect, const CString& text, DWORD dwForeColor, DWORD dwBackColor, DWORD dwBorderColor, int iFont, int iSize, DWORD dwFlags)
{
	DrawFilledBox(rect, dwBackColor, dwBorderColor);
	rect.top +=1;
	rect.bottom -=1;
	rect.left +=1;
	rect.right -=1;
	DrawSomeText(rect, text, dwForeColor, dwBackColor , iFont, iSize, dwFlags);
}

int OSDRegion::GetW(const char *str, BYTE fntType, BYTE fntSize )
{
	CFont* theFont = CreateFont(fntType, fntSize);
	CDC* pDC = GetDC();
	CFont* oldFont = pDC->SelectObject(theFont);
	CSize sz = pDC->GetTextExtent(str);
	pDC->SelectObject(oldFont);
	delete theFont;
	ReleaseDC();
	return sz.cx;
}

void OSDRegion::InvalidateFrameBuffer()
{
	m_pParent->GetFrameBuffer().Invalidate();
}

int OSDRegion::GetXOffs()
{
	return m_bMemRegion ? 0 : m_x;
}

int OSDRegion::GetYOffs()
{
	return m_bMemRegion ? 0 : m_y;
}

bool OSDRegion::AreAnyWindowsUpdated()
{
	return !m_Windows.empty(); // later
}
