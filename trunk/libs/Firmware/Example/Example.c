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
#include "TAPExtensions.h"

TAP_ID( 0x81030504 );
TAP_PROGRAM_NAME("Example");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Sample Firmware library client TAP");
TAP_ETCINFO(__DATE__);

#include "FirmwareCalls.h"


//-----------------------------------------------------------------------------
void TestMove()
{
	TAP_Print("Creating test file\n");
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_Create( "test.txt", ATTR_NORMAL );

	TAP_Print("Move file ");
	TAP_Print("%d\n", TAP_Hdd_Move( "test.txt", "ProgramFiles" ) );
	TAP_Print("Move to non existent directory ");
	TAP_Print("%d\n", TAP_Hdd_Move( "test.txt", "Blah" ) );
	TAP_Print("Move non existent file ");
	TAP_Print("%d\n", TAP_Hdd_Move( "test.txt", "ProgramFiles" ) );
	TAP_Print("Move non existent file ");
	TAP_Print("%d\n", TAP_Hdd_Move( "blah.txt", "Blah" ) );
}


//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_IDLE )
	{
	}
	else if ( event == EVT_KEY )
	{
		if ( param1 == RKEY_Power )
		{
			TAP_Print("Exiting Example\n");
			FreeMoveFunction();
			TAP_Exit();
			return 0;
		}
		if ( param1 == RKEY_Recall )
		{
			TestMove();
			return 0;
		}
	}

	return param1;
}

//-----------------------------------------------------------------------------
int TAP_Main()
{
	InitMoveFunction();

	TestMove();

	return 1; 
}
