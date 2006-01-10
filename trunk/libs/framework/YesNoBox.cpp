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

#include "tap.h"
#include <string.h>
#include "YesNoBox.h"
#include "../../../trunk/libs/framework/Tapplication.h"
#include "TextTools.h"
#include "graphics.c"



YesNoBox::YesNoBox(const char* title, const char* line1, const char* line2, const char* button1, const char* button2, int defaultOption) 
: DialogBox(title, line1, line2)
{
	option		= defaultOption;
	result		= false;
	this->button1 = button1;
	this->button2 = button2;

	if (this->button2.empty())
		option = 0;
}


YesNoBox::~YesNoBox()
{
}


void YesNoBox::CreateDialog()
{
	DialogBox::CreateDialog();
	DisplayLine();
}

//------------------------------------------------------------------
//
void YesNoBox::DisplayLine()
{
	int offset = 0;
	if (button2.empty())
	{
		offset = YESNO_OPTION_X_SPACE/2;
		option = 0;
	}

	TAP_Osd_PutGd( GetTAPScreenRegion(), YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE) + offset, YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(GetTAPScreenRegion(), YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE) + offset, YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W + offset, button1, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	if (!button2.empty())
	{
		TAP_Osd_PutGd( GetTAPScreenRegion(), YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
		PrintCenter(GetTAPScreenRegion(), YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button2, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	}

	switch ( option )
	{

	case 0 :
		TAP_Osd_PutGd( GetTAPScreenRegion(), YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE) + offset, YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
		PrintCenter(GetTAPScreenRegion(), YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE) + offset, YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W + offset, button1, COLOR_Yellow, 0, FNT_Size_1926 );
		break;

	case 1 :
		TAP_Osd_PutGd( GetTAPScreenRegion(), YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
		PrintCenter(GetTAPScreenRegion(), YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, button2, COLOR_Yellow, 0, FNT_Size_1926 );
		break;

	}
}


//------------
//
dword YesNoBox::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_VolUp:	
		if ( option < 1 ) 
			option++;
		else 
			option = 0;
		DisplayLine();
		break;

	case RKEY_VolDown:	
		if ( option > 0 ) 
			option--;
		else 
			option = 1;
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
		EndDialog(result ? Dialog_Ok : Dialog_Cancel);
		break;

	case RKEY_Exit :
		result = false;
		EndDialog(Dialog_Cancel);
		break;

	case RKEY_Mute :	return key;
	}

	return 0;
}
