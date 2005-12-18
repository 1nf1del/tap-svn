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
#include "YesNoBox.h"
#include "TextTools.h"

#include "graphics.c"

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


YesNoBox::YesNoBox(char* title, char* line1, char* line2, char* button1, char* button2, int defaultOption, YesNoCallback callback ) :
	_callback( callback )
{
	this->title		= title;
	this->line1		= line1;
	this->line2		= line2;
    option		= defaultOption;
    result		= false;
    strcpy(this->button1, button1);
    strcpy(this->button2, button2);
}


YesNoBox::~YesNoBox()
{
}


void YesNoBox::OnOpen()
{
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    windowCopy = TAP_Osd_SaveBox(screenRgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H);

    // Display the pop-up window.
    TAP_Osd_PutGd( screenRgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, &_popup360x180Gd, TRUE );

    // Display Title and information in pop-up window
	PrintCenter( screenRgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  13, YESNO_WINDOW_X+YESNO_WINDOW_W-5, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	PrintCenter( screenRgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  56, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line1, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter( screenRgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  89, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line2, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

    DisplayLine();
}


//-----------------------------------------------------------------------
//
void YesNoBox::OnClose()
{
	TAP_Osd_RestoreBox(screenRgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H, windowCopy);
	TAP_MemFree(windowCopy);

	if ( _callback && result )
	{
		// commented out as it doesn't work at the moment
		_callback( result );
	}
}


//------------------------------------------------------------------
//
void YesNoBox::DisplayLine()
{
	TAP_Osd_PutGd( screenRgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
    PrintCenter(screenRgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button1, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( screenRgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(screenRgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button2, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	switch ( option )
	{

		case 0 :
					TAP_Osd_PutGd( screenRgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
				    PrintCenter(screenRgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button1, COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :
					TAP_Osd_PutGd( screenRgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
					PrintCenter(screenRgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button2, COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
	}
}
                                       
                                           
//------------
//
dword YesNoBox::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
		case RKEY_VolUp:	if ( option < 1 ) option++;
							else option = 0;
							DisplayLine();
		     				break;

		case RKEY_VolDown:	if ( option > 0 ) option--;
							else option = 1;
							DisplayLine();
							break;

		case RKEY_Ok :		
                            switch ( option )
							{
								case 0 :   result = true;
                                           break;	// YES

								case 1 :   result = false;
                                           break;	// NO

								default :  result = false;
                                           break;
							}
							Close();		    // Close the yes/no window
							break;

		case RKEY_Exit : 	
							result = false;
                            Close();					// Close the edit window
							break;

		case RKEY_Mute :	return key;
	}
	return 0;
}
