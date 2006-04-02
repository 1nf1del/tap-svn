/*
	Copyright (C) 2005 Simon Capewell

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

#include <string.h>
#include <tap.h>
#include "extap.h"
#include "Firmware.h"
#include "tapapifix.h"
#include "TAPExtensions.h"
#include "OpCodes.h"
#include "ScreenCapture.h"


TAP_ID( 0x810a0013 );
TAP_PROGRAM_NAME("Virtual Remote 1.0");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Control your Toppy from your PC via the serial cable");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.h"


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
	// print exit message
	TAP_Print(" \nVirtual Remote TAP closed\n ");

	return TRUE;
}


void TSRCommanderConfigDialog()
{
}


bool IsRecording(int slot)
{
	TYPE_RecInfo recInfo;
	memset( &recInfo, 0, sizeof(recInfo) );
	return TAP_Hdd_GetRecInfo(slot, &recInfo) &&
		(recInfo.recType == RECTYPE_Normal || recInfo.recType == RECTYPE_Copy);
}


typedef struct _Command
{
	byte length;
	byte crc;
	word command;
	dword param1;
	dword param2;
	byte id;
} Command;

// Process serial input
void ProcessInput()
{
	int i;
	Command cmd;
	byte* buffer = (byte*)&cmd;
	byte crc = 0;
	for ( i=0; i<13; ++i )
		buffer[i] = TAP_GetCh();
	for ( i=3; i<13; ++i )
		crc += buffer[i];

	// Send a reply
	TAP_PutByte(236);
	TAP_PutByte(10);
	TAP_PutByte(0);
	if ( cmd.crc == crc )
	{
		// Success reply
		TAP_PutByte(1);
		TAP_PutByte(236);
		switch ( cmd.command )
		{
		case 1:
			if ( cmd.param2 == 0 )
				TAP_GenerateEvent( EVT_KEY, cmd.param1, 0 );
			else
			{
				if ( cmd.param2 & 0x100 )
					cmd.param2 = 0x10000 | (cmd.param2 & 0xff);
				exTAP_CallFirmware( (void*)0x80005bb4, 0xe300, cmd.param2, 0, 0 );
			}
			break;
		case 2:
		{
			ScreenCaptureFlags flags = cmd.param1;
			if ( flags == 0 )
				flags = SCREENCAP_TV | SCREENCAP_OSD | SCREENCAP_ALPHA;
			CaptureScreenToFile(flags);
			break;
		}
		case 3:
		{
			// no check for running recordings needed, this has the same effect as
			// pressing the power button
			exTAP_Shutdown();
			break;
		}
		case 4:
		{
			if ( IsRecording(0) || IsRecording(1) )
				TAP_Print("Not rebooting, recording in progress\n");
			else
			{
				dword cmdBlock[10];
				cmdBlock[0] = 1;
				cmdBlock[1] = 4;
				exTAP_CallFirmware( (void*)0x80000c68, 14, (dword)cmdBlock, 0, 0);
			}
			break;
		}
		case 5:
		{
			//dword addr = exTAP_TAP_Load(line+7);
			//if ( addr )
			//	TAP_Print("Loaded %s at %08X\n", line+7, addr);
			//else
			//	TAP_Print("Failed to load %s\n", line+7);
			break;
		}
		case 6:
		{
			dword size, free, available;
			TAP_MemInfo( &size, &free, &available );
			TAP_Print("Memory size:%d free:%d available:%d\n", size, free, available);
			break;
		}
		case 7:
		{
			dword state, substate;
			TAP_GetState( &state, &substate );
			TAP_Print("State:%d substate:%d\n", state, substate);
			break;
		}
		case 8:
		{
			TAP_Print("Virtual remote: TAP_SetSystemVar(%d,%d)\n", cmd.param1, cmd.param2);
			TAP_SetSystemVar( cmd.param1, cmd.param2 );
			break;
		}
		default:
			TAP_Print("Virtual remote: Unknown command: %d (%d,%d)\n", cmd.command, cmd.param1, cmd.param2);
			break;
		}
	}
	else
	{
		// Failure reply
		TAP_PutByte(0);
		TAP_PutByte(236);
		TAP_Print("Virtual remote: Ignored command with bad CRC\n");
	}
}


//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_IDLE )
	{
		// When the Stop Recording? or Check your reservation messages are shown, EVT_UART isn't passed to the TAP.
		if ( TAP_KbHit() > 0 )
		{
			if ( TAP_GetCh() == 236 )
			{
				ProcessInput();
				return 0;
			}
		}
	}
	if ( event == EVT_UART )
	{
		if ( param1 == 236 )
		{
			ProcessInput();
			return 0;
		}

		return param1;
	}

	return param1;
}



//-----------------------------------------------------------------------------
int TAP_Main()
{
	exTAP_Init();

	// print start message
	TAP_Print("\nVirtual Remote TAP started\n\n");

	return 1;
}
