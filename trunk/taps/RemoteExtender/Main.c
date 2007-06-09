/*
	Copyright (C) 2007 Simon Capewell

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
#include <model.h>
#include "RemoteExtender.h"
#include "MHEGState.h"
#include "Settings.h"


#ifdef TOPPY2
#define TOPPY2_TEXT "Toppy2 " 
#else
#define TOPPY2_TEXT ""
#endif


TAP_ID(0x814243a3);
TAP_PROGRAM_NAME("Remote Extender " TOPPY2_TEXT "1.51");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Makes extra remote keys accessible to other TAPs");
TAP_ETCINFO(__DATE__);


#include <TSRCommander.h>


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
#ifdef DEBUG
	TAP_Print("Exiting RemoteExtender\n");
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
	if ( event == EVT_KEY-1 )
	{
		if ( OptionsMenu_IsActive() )
			return OptionsMenu_HandleKey( param1, param2 );
	}

	return RemoteExtender_EventHandler( event, param1, param2 );
}


void ShowUnsupportedMessage()
{
	// Try and get some helpful information of users with unsupported firmware
	char buffer[300];
	sprintf( buffer, "%s is not compatible with your firmware\n"
		"For an update, please contact the author quoting\n"
		"%d and %04X", __tap_program_name__, *sysID, _appl_version );
	ShowMessage( buffer, 750 );
}


int TAP_Main(void)
{
	TAP_Print( "Starting Remote Extender TAP\n" );

	Settings_Load();

	if ( !RemoteExtender_Init() )
	{
		ShowUnsupportedMessage();
		return 0;
	}

	MHEGState_Init();
	if ( settings.mheg )
		MHEGState_Enable();

	QuickAspectBlocker_Init();
	if ( settings.quickAspectBlocker )
		QuickAspectBlocker_Enable();

	return TAP_HASCONFIGMENU;
}
