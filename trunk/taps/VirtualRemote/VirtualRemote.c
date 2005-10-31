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
#include "Firmware.h"
#include "OpCodes.h"


TAP_ID( 0x810a0012 );
TAP_PROGRAM_NAME("Virtual Remote");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Control your Toppy from your PC via the serial cable");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.h"

#define RX_BUF_SIZE 4096

static bool echo = TRUE;
static char rxBuffer[RX_BUF_SIZE];
static int writeIndex = 0;
static int readIndex = 0;

dword kbHitCall;

typedef void (*CmdParser)(char* cmdLine);
CmdParser cmdParser = NULL;


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
	UndoFirmwareHacks();

	// free the code wrapper buffer
	free( cmdParser );

	// print exit message
	TAP_Print(" \nVirtual Remote TAP closed\n ");

	return TRUE;
}


void TSRCommanderConfigDialog()
{
}

//-----------------------------------------------------------------------------
// This function processes the input buffer to get a line terminated by a carrige return (CR).
// On occurence of CR the line is passed to the extended command handler in the firmware.
void ProcessInput()
{
	static char line[255];
	static int length = 0;
	char ch;

	while ( readIndex != writeIndex )
	{
		ch = rxBuffer[readIndex];

		// check the line length
		if ( length >= (sizeof(line) - 1) )
		{
			TAP_Print( "\nCommand line too long!\n" );
			length = 0;

			// display new prompt
			if ( echo )
				TAP_Print( "\n=> " );
		}

		switch(ch)
		{
		case '\r':
			// CR found
			if ( !length )
			{
				// display new prompt
				if ( echo )
					TAP_Print("\n=> ");

				break;
			}

			// terminate string
			line[length] = 0;

			if ( echo )
				TAP_Print("\n");

			// check for exit command
			if ( stricmp(line, "exit") == 0 )
			{
				TSRCommanderExitTAP();
				TAP_Exit();
				return;
			}
			else if ( stricmp(line, "echo on") == 0 )
			{
				echo = TRUE;
			}
			else if ( stricmp(line, "echo off") == 0 )
			{
				echo = FALSE;
			}
			else if ( stricmp(line, "timeshift off") == 0)
			{
				TAP_SetSystemVar( SYSVAR_Timeshift, 0 );
			}
			else if ( stricmp(line, "timeshift on") == 0)
			{
				TAP_SetSystemVar( SYSVAR_Timeshift, 1 );
			}
			else if ( strcmpi(line, "key newf1") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_NewF1, 0 );
			}
			else if ( stricmp(line, "key <<") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Prev, 0 );
			}
			else if ( stricmp(line, "key >>") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Next, 0 );
			}
			else if ( stricmp(line, "key sat") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Sat, 0 );
			}
			else // otherwise call command handler
				cmdParser( line );

			// display new prompt
			if ( echo )
				TAP_Print("\n=> ");

			// reset line
			length = 0;
			break;
		case '\b':
			// backspace
			if ( length > 0)
			{
				if ( echo)
					TAP_Print("\b \b");
				length--;
			}
			break;
		default:
			if ( (length == 0 && ch == 0x20) || ch < 0x20 || ch > 0x7e )
			{
				// ignore character
				break;
			}

			// display character
			if ( echo )
				TAP_PutCh( ch );

			// copy character to line buffer
			line[length] = ch;
			length++;
		}

		readIndex = (readIndex + 1) % RX_BUF_SIZE;
	}
}

//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_IDLE)
	{
		int *preadIndex = &readIndex;
		int *pwriteIndex = &writeIndex;
		char *pRxBuffer = rxBuffer;

		// read from serial port until empty
		while ( TAP_KbHit() > 0 )
		{
			// read character to buffer
			pRxBuffer[*pwriteIndex] = TAP_GetCh();

			// store character in buffer
			*pwriteIndex = (*pwriteIndex + 1) % RX_BUF_SIZE;

			// check for overflow
			if (*pwriteIndex == *preadIndex)
			{
				// increment the readIndex
				*preadIndex = (*preadIndex + 1) % RX_BUF_SIZE;
			}
		}

		ProcessInput();
	}
	else if ( event == EVT_UART)
	{
		return 0;
	}

	return param1;
}

//-----------------------------------------------------------------------------
// firmware signature of the command parser function
// bytes with the value of 0xff are not compared
dword cmdParserSignature[] = {
	0x27bdffe0,
	0xafb60018,
	0xafbe001c,
	0xafbf0014,
	0x0080f025,
	0x3c0480ff,
	0x2484ffff,
	0x03c02825,
	0x0c08ffff,
	0x00000000,
	0x0040b025,
	0x2418ffff,
	0x12d80009,
	0x00000000,
	0x3c0480ff,
	0x2484ffff,
	0x02c02825,
	0x03c03025,
	0x0c08ffff,
	0x00000000,
	0x10000006,
	0x00000000,
	0x3c0480ff,
	0x2484ffff,
	0x03c02825,
	0x0c02ffff,
	0x00000000,
	0x8fbf0014,
	0x8fb60018,
	0x8fbe001c,
	0x27bd0020,
	0x03e00008,
	0x00000000
};


dword systemEventProcSignature[] = 
{
	0x27bdff90,
	0xafbf0058,
	0xafb3005c,
	0xafb40060,
	0xafb50064,
	0xafb60068,
	0x0cffffff,
	0xafbe006c,
	0x0cffffff,
	0x00000000
};


// Get the dword representing a call to the internal kbhit function
dword GetKbHitCall()
{
	int i;
	dword* addr;

	// Search for the first call in TAP_KbHit. This should be the internal kbhit function
	addr = (dword*)TAP_KbHit;
	for ( i = 0; i < 40 && *addr != JR_CMD; ++i )
	{
		if ( (*addr & JAL_MASK) == JAL_CMD )
			return *addr;
		++addr;
	}

	TAP_Print( "Could not locate internal KbHit function %04X\n", _appl_version );
	return 0;
}


// Steal serial input from the firmware by removing calls to kbhit within TAP_SystemProc and
// SystemEventProc
bool StealSerialInput()
{
	dword* addr;

	dword kbHitCall = GetKbHitCall();
	if ( !kbHitCall )
		return FALSE;

	// Patch TAP_SystemProc
	FindAndHackFirmware( (dword*)TAP_SystemProc, 40, kbHitCall, 0x0001025 );

	addr = (dword*)FindFirmwareFunction( 
		systemEventProcSignature, sizeof(systemEventProcSignature), 0x800ff000, 0x80110000 );
	if ( addr == 0 )
	{
		TAP_Print( "Could not locate SystemEventProc function %04X\n", _appl_version );
		return FALSE;
	}

	FindAndHackFirmware( addr, 500, kbHitCall, 0x0001025 );

	return TRUE;
}


//-----------------------------------------------------------------------------
int TAP_Main()
{
	dword cmdParserAddr;

	if ( !StealSerialInput() )
		return 0;

	cmdParserAddr = FindFirmwareFunction( cmdParserSignature, sizeof(cmdParserSignature), 0x80108000, 0x80110000 );
	if ( cmdParserAddr == 0 )
		cmdParserAddr = FindFirmwareFunction( cmdParserSignature, sizeof(cmdParserSignature), 0x80230000, 0x80248000 );
	if ( cmdParserAddr == 0 )
	{
		TAP_Print( "Could not locate command interpretter %04X\n", _appl_version );
		return 0;
	}

	cmdParser = (CmdParser)CreateAPIWrapper(cmdParserAddr);
	if ( cmdParser == NULL )
	{
		TAP_Print( "Wrapper initialization failed\n" );
		return 0;
	}

	// print start message
	TAP_Print("\nVirtual Remote TAP started\n\n");
	TAP_Print("=> ");

	return 1;
}
