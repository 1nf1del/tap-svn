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

TAP_ID( 0x810a0012 );
TAP_PROGRAM_NAME("Virtual Remote");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Control your Toppy from your PC via the serial cable");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.inc"

extern int _appl_version;

#define RX_BUF_SIZE 4096

static bool echo = TRUE;
static char rxBuffer[RX_BUF_SIZE];
static int writeIndex = 0;
static int readIndex = 0;

dword kbHitCall;
dword* replacedAddress[2];

typedef void (*CmdParser)(char* cmdLine);
CmdParser cmdParser = NULL;

#define JAL_CMD 0x0c000000
#define JR_CMD 0x03e00008
#define JAL_MASK 0xff000000

//-----------------------------------------------------------------------------
// InitCodeWrapper() creates a code wrapper based on TAP_GetTime. The function
// allocates a buffer and copies the code of TAP_GetTime into it except for
// the actual call of the getTime() firmware function. The address of the 
// getTime() is replaced with the provided address.
// The wrapped function call supports up to 4 parameters and returns a
// short value.
// Parameters:
//      fwFuncAddr - address of the FW function to be wrapped
// InitCodeWrapper() returns a buffer with code wrapping the call to provided
// firmware function address.
// The buffer can be assigned to a function pointer which can be used to execute
// the wrapped code.
dword* InitCodeWrapper( dword fwFuncAddr )
{
	int i;
	// TAP_GetTime provides a template for wrapper code. The only thing to be
	// changed is the address of the wrapped function.
	dword *pSrc = (dword*)TAP_GetTime;
	dword *pBuffer;
	int bufSize = 0;

	// find the return statement
	while ( pSrc[bufSize] != JR_CMD )
	{
		bufSize++;
	}

	// the buffer should include the return statement and the command following
	// the return statement
	bufSize += 2;

	// allocate memory
	pBuffer = malloc( sizeof(dword) * bufSize );

	if ( pBuffer == NULL )
	{
		return NULL;
	}

	for ( i = 0; i < bufSize; i++, pSrc++ )
	{
		if ( (*pSrc & JAL_MASK) == JAL_CMD )
		{
			// this is the wrapped call to the actual FW implementation
			// replace it with the call to the provided address
			pBuffer[i] = JAL_CMD | ((fwFuncAddr & ~JAL_MASK) >> 2);
		}
		else
		{
			pBuffer[i] = *pSrc;
		}
	}

	return pBuffer;
}


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
void CleanUp()
{
	if ( replacedAddress[0] )
		*replacedAddress[0] = kbHitCall;
	if ( replacedAddress[1] )
		*replacedAddress[1] = kbHitCall;

	// free the code wrapper buffer
	free( cmdParser );

	// print exit message
	TAP_Print(" \nRemote Keyboard TAP closed\n ");
}


//----------------------------------------------------------------------------
// No configuration to display
void TSRCommanderConfigDialog()
{
}


//----------------------------------------------------------------------------
// Cleanup and exit
bool TSRCommanderExitTAP()
{
	CleanUp();

	return TRUE;
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
				CleanUp();
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

		TSRCommanderWork();
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
	0x0c06ffff,
	0xafbe006c,
	0x0c0015dc,
	0x00000000,
	0x3c058025,
	0x24a5ffff,
	0x3c044005,
	0x0c06ffff,
	0x34840100,
	0x0c06ffff,
	0x00000000,
	0x0c06ffff,
	0x00000000,
	0x04410004,
	0x00000000,
	0x3c048025,
	0x0c02ffff,
	0x2484ffff,
	0x0c06ffff,
	0x00000000,
	0xaf828514,
	0x0c07ffff,
	0xa3809d01
};


dword FindFirmwareFunction( dword* signature, size_t signatureSize, dword start, dword end )
{
	byte* p;
	for ( p = (byte*)start; p < (byte*)end; ++p )
	{
		dword address = (dword)p;
		byte* sig = (byte*)signature;
		int i;
		for ( i = 0; i < signatureSize && p < (byte*)end && (*sig==*p || *sig==0xff); ++i )
		{
			++sig;
			++p;
		}
		if ( i >= signatureSize )
			return address;
	}

	return 0;
}


// Find and replace a single instruction within an address range or function
dword* ReplaceInstruction( dword* start, dword length, dword oldOpCode, dword newOpCode )
{
	int i;
	for ( i = 0; i < length && *start != JR_CMD; ++i )
	{
		if ( *start == oldOpCode )
		{
			*start = newOpCode;
			return start;
		}
		++start;
	}
	return NULL;
}


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
	replacedAddress[0] = ReplaceInstruction( (dword*)TAP_SystemProc, 40, kbHitCall, 0x0001025 );

	addr = (dword*)FindFirmwareFunction( 
		systemEventProcSignature, sizeof(systemEventProcSignature), 0x800ff000, 0x80110000 );
	if ( addr == 0 )
	{
		TAP_Print( "Could not locate SystemEventProc function %04X\n", _appl_version );
		return FALSE;
	}

	replacedAddress[1] = ReplaceInstruction( addr, 500, kbHitCall, 0x0001025 );

	return TRUE;
}


//-----------------------------------------------------------------------------
int TAP_Main(void)
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

	cmdParser = (CmdParser)InitCodeWrapper(cmdParserAddr);
	if ( cmdParser == NULL )
	{
		TAP_Print( "Wrapper initialization failed\n" );
		return 0;
	}

	// print start message
	TAP_Print("\nVirtual Remote TAP started\n\n");
	TAP_Print("=> ");

	TSRCommanderInit( 0, FALSE );

	return 1;
}
