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

#include <tap.h>
#include <string.h>
#include "Tapplication.h"
#include "BitmapPage.h"


BitmapPage::BitmapPage( TYPE_GrData* gd )
{
	this->gd = gd;
	this->width = gd->width;
	this->height = gd->height;
	this->pixmapData = NULL;
}


BitmapPage::BitmapPage( int width, int height, word* pixmapData )
{
	this->gd = NULL;
	this->width = width;
	this->height = height;
	this->pixmapData = pixmapData;
}

BitmapPage::~BitmapPage()
{
}


void BitmapPage::OnOpen()
{
	int x = (MAX_SCREEN_X - width) / 2;
	int y = (MAX_SCREEN_Y - height) / 2;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
	windowCopy = TAP_Osd_SaveBox(GetTAPScreenRegion(), x,y, width, height);
	// Display the pop-up window.
	if ( gd )
		TAP_Osd_PutGd( GetTAPScreenRegion(), x,y, gd, TRUE );
	else if ( pixmapData )
		TAP_Osd_DrawPixmap( GetTAPScreenRegion(), x,y, width,height, pixmapData, TRUE, OSD_1555 );
}


void BitmapPage::OnClose()
{
	int x = (MAX_SCREEN_X - width) / 2;
	int y = (MAX_SCREEN_Y - height) / 2;

	TAP_Osd_RestoreBox(GetTAPScreenRegion(), x,y, width,height, windowCopy);
	TAP_MemFree(windowCopy);
}


dword BitmapPage::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Ok:
	case RKEY_Exit:
		Close();		    // Close window
		break;

	case RKEY_Mute:
		return key;
	}
	return 0;
}
