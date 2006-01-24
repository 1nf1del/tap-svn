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
#ifndef _rect_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#define _rect_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h

#include <stdlib.h>
#include "tap.h"

class Rect
{
public:
	Rect(short x, short y, short w, short h);
	Rect();

	void Shrink(int i);
	int GetWidth(const char* text, byte fntSize) const;
	void PutString(word regionIndex, const char* text, word foreground, word background, byte fntSize, bool bCenter = false) const;
	void PutStringCenter(word regionIndex, const char* text, word foreground, word background, byte fntSize) const;
	void PutMultiLineString(word regionIndex, const char* text, word foreground, word background, byte fntSize) const;
	void Fill(word regionIndex, word color) const;
	void DrawBox(word regionIndex, short int thickness, word color) const;
	word CreateRegion(bool bMemRegion = false);
	void DrawLowerBorder(word regionIndex, short int thickness, word color) const;
	void DrawRightBorder(word regionIndex, short int thickness, word color) const;

	static void SetOffset(short x, short y);

	short int  x;
	short int y;
	short int w;
	short int h;

	
private:
	// screen offset
	static short offsetX, offsetY;
};


inline void Rect::SetOffset(short x, short y)
{
	offsetX = x;
	offsetY = y;
}


#endif
