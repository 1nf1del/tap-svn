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

// This TAP outputs all public TAP API addresses to a file and the Toppy serial port
// Possible enhancements:
// Output a .map file for input into disasmips

#include <tap.h>

TAP_ID( 0x8994fc12 );
TAP_PROGRAM_NAME("Dump TAP API addresses");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Outputs TAP API addresses to a csv file");
TAP_ETCINFO(__DATE__);

dword getgd()
{
	__asm__ __volatile__ (
		"lw	$2,624($28)\n"
		"nop\n"				
		"lw	$2,0($2)\n"
	);
}

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
}


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


// Missing TAP API header definitions
extern int _appl_version;
extern int __startPoint;
dword TAP_GetSystemProc();


char fileBuffer[4096];

#define JAL_CMD 0x0c000000
#define JR_CMD 0x03e00008
#define JAL_MASK 0xff000000

void OutputAPI( char* name, dword* address )
{
	char buffer[200];
	int i;

	sprintf( buffer, "%s, %08X", name, address );
	TAP_Print( buffer );
	strcat( fileBuffer, buffer );

	for ( i = 0; i < 500 && *address != JR_CMD; ++address, ++i )
	{
		if ( (*address & JAL_MASK) == JAL_CMD )
		{
			sprintf( buffer, ", %08X", (*address & ~JAL_MASK) << 2 );
			TAP_Print( buffer );
			strcat( fileBuffer, buffer );
		}
		++address;
	}
	sprintf( buffer, "\n" );
	TAP_Print( buffer );
	strcat( fileBuffer, buffer );
}

#define Output(fn) \
	sprintf( buffer, #fn ", %08X\n", fn ); \
	TAP_Print( buffer ); \
	strcat( fileBuffer, buffer );

#define OutputAPI(fn) OutputAPI(#fn, (dword*)fn)

#define OutputHack(fn) \
	sprintf( buffer, "%s, %08X\n", fn, addr ); \
	TAP_Print( buffer ); \
	strcat( fileBuffer, buffer );

int TAP_Main(void)
{
	char buffer[200];
	dword addr;
	TYPE_File* fp;
	char filename[TS_FILE_NAME_SIZE];

	sprintf( buffer, "_appl_version, %04X\n", _appl_version );
	TAP_Print( buffer );
	strcat( fileBuffer, buffer );

	Output( _tap_startAddr );
	Output( __startPoint );
	Output( TAP_GetSystemProc );

	OutputAPI( TAP_SystemProc );
	OutputAPI( TAP_GetState );
	OutputAPI( TAP_ExitNormal );
	OutputAPI( TAP_EnterNormal );
	OutputAPI( TAP_Exit );

	OutputAPI( TAP_KbHit );
	OutputAPI( TAP_GetCh );
	OutputAPI( TAP_PutCh );
	OutputAPI( TAP_PutByte );
	OutputAPI( TAP_PrintSys );
	OutputAPI( TAP_Print );

	OutputAPI( TAP_SysOsdControl );
	OutputAPI( TAP_SetBaudRate );
	OutputAPI( TAP_SetBk );

	// -- COMMON FUNCTION ---------------------------------------------------

	OutputAPI( TAP_SetSoundLevel );
	OutputAPI( TAP_GetTime );
	OutputAPI( TAP_MemInfo );
	OutputAPI( TAP_MemAlloc );
	OutputAPI( TAP_MemFree );
	OutputAPI( TAP_GetTick );
	OutputAPI( TAP_Delay );
	OutputAPI( TAP_ExtractMjd );
	OutputAPI( TAP_MakeMjd );
	OutputAPI( TAP_Sin );
	OutputAPI( TAP_Cos );
	OutputAPI( TAP_SPrint );
	OutputAPI( TAP_GetSignalLevel );
	OutputAPI( TAP_GetSignalQuality );
	OutputAPI( TAP_SetInfoboxTime );
	OutputAPI( TAP_PlayPCM );
	OutputAPI( TAP_CaptureScreen );

	OutputAPI( TAP_SetSystemVar );
	OutputAPI( TAP_GetSystemVar );
	OutputAPI( TAP_WriteSystemVar );
	OutputAPI( TAP_GenerateEvent );

	// -- OSD FUNCTION ---------------------------------------------------
	OutputAPI( TAP_Osd_Create );
	OutputAPI( TAP_Osd_Delete );
	OutputAPI( TAP_Osd_Move );
	OutputAPI( TAP_Osd_FillBox );
	OutputAPI( TAP_Osd_PutBox );
	OutputAPI( TAP_Osd_GetBox );
	OutputAPI( TAP_Osd_SetLut );
	OutputAPI( TAP_Osd_Ctrl );
	OutputAPI( TAP_Osd_SetTransparency );

	OutputAPI( TAP_Osd_Draw3dBox );
	OutputAPI( TAP_Osd_Draw3dBoxFill );

	OutputAPI( TAP_Osd_DrawRectangle );
	OutputAPI( TAP_Osd_DrawPixmap );
	OutputAPI( TAP_Osd_Zoom );
	OutputAPI( TAP_Osd_GetBaseInfo );
	OutputAPI( TAP_Osd_Copy );

	OutputAPI( TAP_Osd_SaveBox );
	OutputAPI( TAP_Osd_RestoreBox );

	OutputAPI( TAP_Osd_GetPixel );
	OutputAPI( TAP_Osd_PutPixel );

	OutputAPI( TAP_Osd_PutGd );

	// -- HDD FUNCTION ---------------------------------------------------

	OutputAPI( TAP_Hdd_Fopen );
	OutputAPI( TAP_Hdd_Fread );
	OutputAPI( TAP_Hdd_Fwrite );
	OutputAPI( TAP_Hdd_Fclose );
	OutputAPI( TAP_Hdd_FindFirst );
	OutputAPI( TAP_Hdd_FindNext );
	OutputAPI( TAP_Hdd_Fseek );
	OutputAPI( TAP_Hdd_Flen );
	OutputAPI( TAP_Hdd_Ftell );
	OutputAPI( TAP_Hdd_TotalSize );
	OutputAPI( TAP_Hdd_FreeSize );
	OutputAPI( TAP_Hdd_Delete );
	OutputAPI( TAP_Hdd_Exist );
	OutputAPI( TAP_Hdd_Create );
	OutputAPI( TAP_Hdd_ChangeDir );

	OutputAPI( TAP_Hdd_PlayTs );
	OutputAPI( TAP_Hdd_StopTs );
	OutputAPI( TAP_Hdd_PlayMp3 );
	OutputAPI( TAP_Hdd_StopMp3 );
	OutputAPI( TAP_Hdd_Rename );

	OutputAPI( TAP_Hdd_StartRecord );
	OutputAPI( TAP_Hdd_StopRecord );
	OutputAPI( TAP_Hdd_GetRecInfo );
	OutputAPI( TAP_Hdd_GetPlayInfo );
	OutputAPI( TAP_Hdd_GetHddID );

	OutputAPI( TAP_Hdd_ChangePlaybackPos );
	OutputAPI( TAP_Hdd_GotoBookmark );
	OutputAPI( TAP_Hdd_SetBookmark );

	// -- STRING FUNCTION ---------------------
	OutputAPI( TAP_Osd_PutS );
	OutputAPI( TAP_Osd_GetW );
	OutputAPI( TAP_Osd_PutString );
	OutputAPI( TAP_Osd_DrawString );
	OutputAPI( TAP_Osd_PutStringAf );

	// -- WINDOW FUNCTION ---------------------------------------------------

	OutputAPI( TAP_Win_SetTitle );
	OutputAPI( TAP_Win_SetColor );
	OutputAPI( TAP_Win_SetDefaultColor );
	OutputAPI( TAP_Win_Draw );
	OutputAPI( TAP_Win_Create );
	OutputAPI( TAP_Win_Delete );
	OutputAPI( TAP_Win_SetFont );
	OutputAPI( TAP_Win_AddItem );
	OutputAPI( TAP_Win_GetSelection );
	OutputAPI( TAP_Win_SetSelection );
	OutputAPI( TAP_Win_Action );

	OutputAPI( TAP_Win_SetAvtice );
	OutputAPI( TAP_Win_SetDrawItemFunc );

	// ---------------------------------------------------------------------

	OutputAPI( TAP_Channel_GetTotalNum );
	OutputAPI( TAP_Channel_GetFirstInfo );
	OutputAPI( TAP_Channel_GetNextInfo );
	OutputAPI( TAP_Channel_GetInfo );
	OutputAPI( TAP_Channel_GetCurrent );
	OutputAPI( TAP_Channel_GetCurrentSubTV );
	OutputAPI( TAP_Channel_Start );
	OutputAPI( TAP_Channel_Scale );
	OutputAPI( TAP_Channel_IsPlayable );
	OutputAPI( TAP_Channel_Move );
	OutputAPI( TAP_Channel_Stop );
	OutputAPI( TAP_Channel_GetTotalAudioTrack );
	OutputAPI( TAP_Channel_GetAudioTrackName );
	OutputAPI( TAP_Channel_SetAudioTrack );
	OutputAPI( TAP_Channel_GetTotalSubtitleTrack );
	OutputAPI( TAP_Channel_GetSubtitleTrackName );
	OutputAPI( TAP_Channel_SetSubtitleTrack );
	OutputAPI( TAP_Channel_IsStarted );

	// --- EPG FUNCTIONS ---------------------------------------------------

	OutputAPI( TAP_GetEvent );
	OutputAPI( TAP_GetCurrentEvent );
	OutputAPI( TAP_PutEvent );

	OutputAPI( TAP_ControlEit );

	OutputAPI( TAP_EPG_UpdateEvent );
	OutputAPI( TAP_EPG_DeleteEvent );
	OutputAPI( TAP_EPG_GetExtInfo );

	// --- USB FUNCTIONS ---------------------------------------------------

	OutputAPI( TAP_Usb_Read );
	OutputAPI( TAP_Usb_Write );
	OutputAPI( TAP_Usb_PacketRead );
	OutputAPI( TAP_Usb_PacketWrite );
	OutputAPI( TAP_Usb_Cancel );

	// --- TIMER FUNCTIONS ---------------------------------------------------

	OutputAPI( TAP_Timer_GetTotalNum );
	OutputAPI( TAP_Timer_GetInfo );
	OutputAPI( TAP_Timer_Add );
	OutputAPI( TAP_Timer_Modify );
	OutputAPI( TAP_Timer_Delete );

	// --- VFD FUNCTIONS ---------------------------------------------------

	OutputAPI( TAP_Vfd_GetStatus );
	OutputAPI( TAP_Vfd_Control );
	OutputAPI( TAP_Vfd_SendData );

	sprintf( buffer, "\nHacked functions\n" );
	TAP_Print( buffer );
	strcat( fileBuffer, buffer );

	// Hacked 
	addr = FindFirmwareFunction( cmdParserSignature, sizeof(cmdParserSignature), 0x80108000, 0x80110000 );
	if ( addr == 0 )
		addr = FindFirmwareFunction( cmdParserSignature, sizeof(cmdParserSignature), 0x80230000, 0x80248000 );
	OutputHack( "CommandParser" );

	addr = getgd();
	OutputHack( "TAP_Osd_PutGd" );

	// Construct filename from the firmware version number
	sprintf( filename, "API %02X.%02X.csv", (_appl_version >> 8) & 0xff, _appl_version & 0xff );

	// Delete old file if necessary
	if ( !TAP_Hdd_Exist( filename ) )
		TAP_Hdd_Delete( filename );

	// Create a new file
	TAP_Hdd_Create( filename, ATTR_NORMAL );

	if ( fp = TAP_Hdd_Fopen( filename ) )
	{
		TAP_Hdd_Fwrite( fileBuffer, strlen(fileBuffer)+511, 1, fp );
		TAP_Hdd_Fclose( fp );
	}

	return 0;
}
