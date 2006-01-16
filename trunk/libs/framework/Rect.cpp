/*
Copyright (C) 2005 Robin Glover

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

#include "Rect.h"
#include <string.h>
#include <ctype.h>
#include "texttools.h"

#ifndef WIN32
#define _ASSERT(x)
#else 
#include <crtdbg.h>
#include <math.h>
#endif
Rect::Rect(short x, short y, short w, short h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

Rect::Rect()
{
	x=y=w=h=-1;
}

void Rect::Shrink(int i)
{
	this->h -= 2*i;
	this->w -= 2*i;
	this->x += i;
	this->y += i;
}

int GetHeightForFontSize(byte fntSize)
{
	switch (fntSize)
	{
	case FNT_Size_1622:
		return 22;
	case FNT_Size_1926:
		return 26;
	default:
	case FNT_Size_1419:
		return 19;
	}
}

int GetWidthForFontSize(byte fntSize)
{
	switch (fntSize)
	{
	case FNT_Size_1622:
		return 8;
	case FNT_Size_1926:
		return 10;
	default:
	case FNT_Size_1419:
		return 7;
	}
}


void Rect::PutString(word regionIndex, const char* text, word foreground, word background, byte fntSize, bool bCenter) const
{
	if (bCenter)
	{
		PutStringCenter(regionIndex, text, foreground, background, fntSize);
	}
	else
	{
		TAP_Osd_PutString(regionIndex, x+2, y + (h - GetHeightForFontSize(fntSize)) / 2,
			w + x -3, 
			text, foreground, background, 0, fntSize, 0);
	}
}

void Rect::PutStringCenter(word regionIndex, const char* text, word foreground, word background, byte fntSize) const
{
	PrintCenter(regionIndex, x+2, y+ (h - GetHeightForFontSize(fntSize)) / 2
		, w-2, text, foreground, background, fntSize);
}

int StringBreakPoint(char* pStr, int width, byte fntSize)
{
	int avgWidth = GetWidthForFontSize(fntSize);
	int iLen = strlen(pStr);
	int iGuess = min(width/avgWidth, iLen);
	char* pCRPoint = strchr(pStr, '\n');
	int iCRPoint = pCRPoint != 0 ? pCRPoint - pStr : 0x7FFFFFF;

	for (int i=0; i<3; i++)
	{
		char cBackup = pStr[iGuess];
		pStr[iGuess] = 0;
		int wUsed = TAP_Osd_GetW(pStr, 0, fntSize);
		pStr[iGuess] = cBackup;
		if ((iGuess>=iLen) && (wUsed <=width))
		{
			int iResult = min(min(iGuess, iCRPoint), iLen);
			_ASSERT(iResult<=iLen);
			return iResult;
		}

		int iNewGuess = iGuess + (width - wUsed) / avgWidth;

		if (abs(iGuess - iNewGuess)<=1)
		{	
			iGuess = min(iGuess, iNewGuess);
			break;
		}
		iGuess = min(iNewGuess,iLen);
		_ASSERT(iGuess<=iLen);
	}

	if (iGuess>=iCRPoint)
	{
		_ASSERT(iCRPoint<=iLen);
		return iCRPoint;
	}

	for (int i=iGuess; i>0; i--)
	{
		if (pStr[i] == 32)
		{
			_ASSERT(i>0);
			_ASSERT(i<=iLen);
			return i;
		}
	}

	_ASSERT(iGuess<=iLen);
	return iGuess;
}



void Rect::PutMultiLineString(word regionIndex, const char* text, word foreground, word background, byte fntSize) const
{
	char* pText = (char*) text; // we need to change it, but promise to put it back :-)
	int avgWidth = GetWidthForFontSize(fntSize);
	int avgHeight = GetHeightForFontSize(fntSize);
	int hUsed = 0;
	int iChars = strlen(text);
	int iDone = 0;
	while (((hUsed+avgHeight)<this->h) && (iDone < iChars))
	{
		char* buf = pText + iDone;
		int iThisLine = StringBreakPoint(buf, w, fntSize);
		_ASSERT(iThisLine + iDone<=iChars);
		char cBackup = pText[iThisLine + iDone];
		pText[iThisLine + iDone] = 0;
		TAP_Osd_PutString(regionIndex, x+2, y+hUsed, w+x-3, isspace(buf[0]) ? buf+1 : buf,
			foreground, background, 0, fntSize, 0);
		pText[iThisLine + iDone] = cBackup;
		iDone += iThisLine;
		_ASSERT(iDone<=iChars);
		while ((iDone < iChars) && isspace(pText[iDone]))
			iDone++;
		hUsed+=avgHeight;
	}
}

void Rect::Fill(word regionIndex, word color) const
{
	TAP_Osd_FillBox(regionIndex, x, y, w, h, color);
}

void Rect::DrawBox(word regionIndex, short int thickness, word color) const
{
	TAP_Osd_DrawRectangle(regionIndex, x , y , w, h , thickness, color);
}

word Rect::CreateRegion(bool bMemRegion)
{
	return TAP_Osd_Create(bMemRegion ? 0 : x, bMemRegion ? 0 : y, w, h, 0, bMemRegion ? OSD_Flag_MemRgn : 0);
}

void Rect::DrawLowerBorder(word regionIndex, short int thickness, word color) const
{
	TAP_Osd_DrawRectangle(regionIndex, x, y + h - 2 , w, 2,  1, color);
}

void Rect::DrawRightBorder(word regionIndex, short int thickness, word color) const
{
	TAP_Osd_DrawRectangle(regionIndex, x + w -2, y , 2, h,  1, color);
}
