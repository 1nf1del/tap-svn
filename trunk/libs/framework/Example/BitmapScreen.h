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

#ifndef __BITMAPSCREEN_H
#define __BITMAPSCREEN_H

#include "Page.h"


class BitmapScreen : public Page
{
public:
	BitmapScreen(TYPE_GrData* gd );
	BitmapScreen(int width, int height, word* pixmapData);
	virtual ~BitmapScreen();

	virtual dword OnKey( dword key, dword extKey );
	virtual void OnOpen();
	virtual void OnClose();

private:
	byte* windowCopy;
	TYPE_GrData* gd;
	int width,height;
	word* pixmapData;
};


#endif
