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

#include <tap.h>
#include <messagewin.h>
#include <Firmware.h>
#include <TAPExtensions.h>
#include <OpCodes.h>
#include "RemoteMap.h"


TAP_ID(0x814243a3);
TAP_PROGRAM_NAME("Remote Extender 0.3");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Makes extra remote keys accessible to other TAPs");
TAP_ETCINFO(__DATE__);

#include <TSRCommander.h>

dword lastKey = 0;


//-----------------------------------------------------------------------------
// ReceiveKeyHook() stores the RC code received from the front panel processor to the
// memory specified in $t1 ($09).
// The value loaded in $t1 has to be set on TAP start. It has to
// be an absolute value. The hook function is to be inserted into
// the function retrieving the RC code from the front panel processor.
// The first two NOPs can be replaced with the instructions overwritten
// by the installed hook.
void ReceiveKeyHook()
{
	__asm__ __volatile__ (
		"nop\n"
		"nop\n"
		"lui	$09,0x0\n"
		"or		$09,$09,0x0\n"
		"nop\n"
		"ori	$08,$00,0xe300\n"
		"bne	$21,$08,a1\n"
		"nop\n"
		"sw		$15,0($09)\n"
		"a1:\n"
		"nop\n");
}


//-----------------------------------------------------------------------------
dword remoteReceiveSignature[] =
{
	0x27bdffe0,
	0xafb50014,
	0xafb60018,
	0xafbe001c,
	0xafbf0010,
	0x0080f025,
	0x0000a825,
	0x0c0000ff,
	0x00000000,
	0x0040b025,
	0x3c198060,
	0x8f39ffff,
	0x3c188060,
	0x8f18ffff,
	0x13380018,
	0x00000000,
	0x3c198060,
	0x2739ffff,
	0x27390008,
	0x3c188060,
	0x8f18ffff,
	0x0018c040,
	0x0338c821,
	0x97350000
};

bool HookReceiveKeyFunction()
{
	dword* hookFn;
	dword* addr;
	
	addr = (dword*)FindFirmwareFunction( remoteReceiveSignature, sizeof(remoteReceiveSignature), 0x80004000, 0x80007000 );
	if ( !addr )
	{
		ShowMessage( "Receive key function not found\n", 200 );
		return FALSE;
	}

	if ( (addr[0x21] & 0xffffff00) != 0x3c198000 || (addr[0x22] & 0xffffff00) != 0x3c188000 )
	{
		ShowMessage( "Firmware receive function has an unexpected structure\n", 200 );
		return FALSE;
	}

	hookFn = (dword*)ReceiveKeyHook;
	// Validate receive hook function
	if (hookFn[2] != LUI_T1_CMD || hookFn[3] != OR_T1_CMD)
	{
		ShowMessage( "TAP receive key hook function has an unexpected structure\n", 200 );
		return FALSE;
	}

	// save the original instructions to be executed prior to the hook code
	hookFn[0] = addr[0x21];
	hookFn[1] = addr[0x22];

	// initialize memory address
	hookFn[2] = LUI_T1_CMD | (((dword)&lastKey >> 16) & 0xffff);
	hookFn[3] = OR_T1_CMD | ((dword)&lastKey & 0xffff);

	// install the hook
	HackFirmware( addr+0x21, JAL(ReceiveKeyHook) );
	HackFirmware( addr+0x22, NOP_CMD );

	return TRUE;
}


dword dispatchKeySignature[] =
{
	0x8eff001c,
	0x8fff0000,
	0x8eff0004,
	0x03c07825,
	0x00000000,
	0x00000000,
	0x03801025,
	0x00000000,
	0x00000000,
	0x02ffe025,
	0x24040100,
	0x02ff2825,
	0x02fff809,
	0x00003025,
	0xafc20000,
	0x8eff0000,
	0x03c0c025,
	0x00000000,
	0x00000000,
	0x03801025,
	0x02ffe025,
	0x26ff0020,
	0x26ff0001,
	0x2eff0010
};

// Change the firmware key dispatch to send 0xFF rather than 0x100
bool AdjustDispatchKeyFunction()
{
	dword* addr;
	
	addr = (dword*)FindFirmwareFunction( dispatchKeySignature, sizeof(dispatchKeySignature), 0x80128000, 0x80148000 );
	if ( !addr )
	{
		ShowMessage( "Dispatch key function not found\n", 200 );
		return FALSE;
	}

	if ( addr[10] != 0x24040100 )
	{
		ShowMessage( "Dispatch key function has an unexpected structure\n", 200 );
		return FALSE;
	}

	HackFirmware( addr+10, 0x240400ff );

	return TRUE;
}


//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
#ifdef DEBUG
	if ( ( event == EVT_KEY || event == EVT_KEY-1 )&& param1 == RKEY_Recall ) 
	{
		UndoFirmwareHacks();
		TAP_Exit();
		TAP_Print("Exiting Remote Extender TAP\n");
		return 0;
	}
#endif

	// Capture the reassigned remote keypress event
	if ( event == EVT_KEY-1 )
	{
		static bool incall = FALSE;

		if ( !incall ) // prevent re-entrance
		{
			int i;

			incall = TRUE;
			param2 = lastKey ? 0x100 | (lastKey & 0xff) : 0;

			//TAP_Print("Sending %04X %08X %02X\n", event, param1, param2 );

			for ( i = 0 ; i < TAP_MAX; ++i )
			{
				if ( i != *currentTAPIndex )	// don't call ourselves
				{
					dword tempParam1 = param1;
					/* Example code to send a specific key translation to a specific TAP
					   In this case Jag's EPG
					if ( lastKey && tapProcess[i].header && tapProcess[i].header->id == 0x80000102 )
					{
						tempParam1 = keys5000[param2 & 0xff];
					}
					*/
					// TAP_Print("Sending %04X %08X %02X\n", EVT_KEY, tempParam1, param2 );
					tempParam1 = TAP_SendEvent( i, EVT_KEY, tempParam1, param2 );

					// Zero return value should mean don't pass the value on to other TAPs
					// The firmware actually does, so make sure param2 is also zero in this case
					if ( tempParam1 == 0 )
					{
						param1 = 0;
						param2 = 0;
					}
				}
			}
			// Clear the keypress
			lastKey = 0;
			incall = FALSE;
		}
		if ( param1 >= RKEY_Cmd_0 && param1 <= RKEY_Cmd_f )
		{
			ShowMessage(
				"Test key detected. It is advisable to avoid\n"
				"this keycode as it may cause disk corruption\n"
				"when RemoteExtender is not running.", 500 );
			return 0;
		}
		return param1;
	}

	return param1;
}


//-----------------------------------------------------------------------------
void TSRCommanderConfigDialog()
{
}


bool TSRCommanderExitTAP()
{
	UndoFirmwareHacks();
	TAP_Exit();
}


int TAP_Main()
{
	TAP_Print( "Starting Remote Extender TAP\n" );

	if ( !StartTAPExtensions() )
	{
		ShowMessage( "Sorry, Remote Extender is not compatible with your firmware", 200 );
		return 0;
	}

	if ( !HookReceiveKeyFunction() )
	{
		UndoFirmwareHacks();
		return 0;
	}

	if ( !AdjustDispatchKeyFunction() )
	{
		UndoFirmwareHacks();
		return 0;
	}

	return 1;
}
