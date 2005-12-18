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
#include <Tapplication.h>
#include "BitmapScreen.h"


// Position of the Delete Confirmation Screen
#define YESNO_WINDOW_W 360
#define YESNO_WINDOW_H 180
#define YESNO_WINDOW_X ((MAX_SCREEN_X-YESNO_WINDOW_W)/2)   // Centre the yes/no window
#define YESNO_WINDOW_Y ((MAX_SCREEN_Y-YESNO_WINDOW_H)/2)   // Centre the yes/no window

#define YESNO_OPTION_W 102                                  // Width  of yes/no Options Buttons
#define YESNO_OPTION_H 43                                   // Height of yes/no Options Buttons
#define YESNO_OPTION_X  (YESNO_WINDOW_X + 52)              // Starting x-position for first Option button
#define YESNO_OPTION_Y  (YESNO_WINDOW_Y + YESNO_WINDOW_H - YESNO_OPTION_H - 15)             // Starting y-position for Option buttons.
#define YESNO_OPTION_X_SPACE   (YESNO_OPTION_W+50)         // Space between options on yes/no window.


BitmapScreen::BitmapScreen(TYPE_GrData* gd )
{
	this->gd = gd;
	this->width = gd->width;
	this->height = gd->height;
	this->pixmapData = NULL;
}


BitmapScreen::BitmapScreen(int width, int height, word* pixmapData)
{
	this->gd = NULL;
	this->width = width;
	this->height = height;
	this->pixmapData = pixmapData;
}

BitmapScreen::~BitmapScreen()
{
}


void BitmapScreen::OnOpen()
{
	int x = (MAX_SCREEN_X - width) / 2;
	int y = (MAX_SCREEN_Y - height) / 2;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
	windowCopy = TAP_Osd_SaveBox(screenRgn, x,y, width, height);
	// Display the pop-up window.
	if ( gd )
		TAP_Osd_PutGd( screenRgn, x,y, gd, TRUE );
	else if ( pixmapData )
		TAP_Osd_DrawPixmap( screenRgn, x,y, width,height, pixmapData, TRUE, OSD_1555 );
}


//-----------------------------------------------------------------------
//
void BitmapScreen::OnClose()
{
	int x = (MAX_SCREEN_X - width) / 2;
	int y = (MAX_SCREEN_Y - height) / 2;

	TAP_Osd_RestoreBox(screenRgn, x,y, width,height, windowCopy);
	TAP_MemFree(windowCopy);
}
                                       
                                           
//------------
//
dword BitmapScreen::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Ok:		
	case RKEY_Exit: 	
		Close();		    // Close the yes/no window
		break;

	case RKEY_Mute:
		return key;
	}
	return 0;
}
