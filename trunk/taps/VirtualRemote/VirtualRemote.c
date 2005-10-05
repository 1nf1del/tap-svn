// TAP to simulate keypresses on the Toppy. Based on DeadBeef's DebugCmds TAP.
// Currently only properly supports the TF5800 5.12.04 firmware

#include <tap.h>
#include <string.h>

TAP_ID( 0x810a0012 );
TAP_PROGRAM_NAME("Virtual Remote");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Control your Toppy from your PC via the serial cable");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.inc"

#define RX_BUF_SIZE 4096

static dword originalEventProc;
static bool echo = TRUE;
static char rxBuffer[RX_BUF_SIZE];
static int writeIndex = 0;
static int readIndex = 0;

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
	*((dword*)0x800fffe8) = originalEventProc;

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
			else if ( strcmpi(line, "virtkey newf1") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_NewF1, 0 );
			}
			else if ( stricmp(line, "virtkey <<") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Prev, 0 );
			}
			else if ( stricmp(line, "virtkey >>") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Next, 0 );
			}
			else if ( stricmp(line, "virtkey sat") == 0 )
			{
				TAP_GenerateEvent( EVT_KEY, RKEY_Sat, 0 );
			}
			else // otherwise call command handler
				cmdParser( line );

			// display new prompt
			if ( echo)
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


dword FindCmdParser( dword start, dword end )
{
	byte* p;
	for ( p = (byte*)start; p < (byte*)end; ++p )
	{
		dword address = (dword)p;
		byte* sig = (byte*)cmdParserSignature;
		int i;
		for ( i = 0; i < sizeof(cmdParserSignature) && p < (byte*)end && (*sig==*p || *sig==0xff); ++i )
		{
			++sig;
			++p;
		}
		if ( i >= sizeof(cmdParserSignature) )
			return address;
	}

	return 0;
}


//-----------------------------------------------------------------------------
int TAP_Main(void)
{
	extern int _appl_version;

	dword cmdParserAddr = FindCmdParser( 0x80108000, 0x80110000 );
	if ( cmdParserAddr == 0 )
		cmdParserAddr = FindCmdParser( 0x80230000, 0x80248000 );
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

	if ( _appl_version == 0x1204 )
	{
		// disable firmware checking for serial activity in the main event loop
		// If any TAPs are calling TAP_SystemProc then this TAP will not run effectively
		originalEventProc = *((dword*)0x800fffe8);
		*((dword*)0x800fffe8) = 0x00001025;
	}

	// print start message
	TAP_Print("\nRemote Keyboard TAP started\n\n");
	TAP_Print("=> ");

	TSRCommanderInit( 0, FALSE );

	return 1;
}

