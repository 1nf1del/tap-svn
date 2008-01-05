/*
	Copyright (C) 2005-2007 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <tap.h>
#include <string.h>
#include <firmware.h>
#include "DescriptionExtender.h"
#include "Settings.h"


#define LOGFILE "DescriptionExtenderLog.txt"

TAP_ID( 0x81243235 );
TAP_PROGRAM_NAME("DescriptionExtender 2.23");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Provides long TAP EPG descriptions");
TAP_ETCINFO(__DATE__);


#include <TSRCommander.h>


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
#ifdef DEBUG
	TAP_Print("Exiting DescriptionExtender\n");
#endif
	UndoFirmwareHacks();
	return TRUE;
}


//-----------------------------------------------------------------------------
// This function displays a config menu
void TSRCommanderConfigDialog()
{
	OptionsMenu_Show();
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_KEY )
	{
		if ( param1 != 0 )
			param1 = OptionsMenu_HandleKey( param1, param2 );
#ifdef DEBUG
		if ( param1 == RKEY_Power )
		{
			TSRCommanderExitTAP();
			TAP_Exit();
			return 0;
		}
#endif
	}
	return param1;
}



int TAP_Main(void)
{
	if (!DescriptionExtender_Init())
		return 0;
	Settings_Load();

	return TAP_HASCONFIGMENU;
}
