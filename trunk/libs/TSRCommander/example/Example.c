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

TAP_ID( 0x81030504 );
TAP_PROGRAM_NAME("Example");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Sample TSR Commander compatible TAP");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.h"

//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
	return TRUE;
}


//-----------------------------------------------------------------------------
// This function displays a config menu if the TAP implements one
void TSRCommanderConfigDialog()
{
}


//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_IDLE )
	{
	}
	else if ( event == EVT_KEY )
	{
	}

	return param1;
}

//-----------------------------------------------------------------------------
int TAP_Main()
{
	// Return 1 if the TAP has no config menu, or TAP_HASCONFIGMENU if it does
	// and you want TSR Commander to show a config menu item for it.
	return 1; 
}
