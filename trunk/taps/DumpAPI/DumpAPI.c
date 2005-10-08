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


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
}


// Missing TAP API header definitions
extern int _appl_version;
extern int __startPoint;
dword TAP_GetSystemProc();

#define Output(fn) \
	sprintf( buffer, #fn ", %08X\n", fn ); \
	TAP_Print( buffer ); \
	TAP_Hdd_Fwrite( buffer, strlen(buffer), 1, fp );


int TAP_Main(void)
{
	char buffer[200];
	TYPE_File* fp;

	// Construct filename from the firmware version number
	char filename[TS_FILE_NAME_SIZE];
	sprintf( filename, "API %02X.%02X.csv", (_appl_version >> 8) & 0xff, _appl_version & 0xff );

	// Delete old file if necessary
	if ( !TAP_Hdd_Exist( filename ) )
		TAP_Hdd_Delete( filename );

	// Create a new file
	TAP_Hdd_Create( filename, ATTR_NORMAL );

	if ( fp = TAP_Hdd_Fopen( filename ) )
	{
		sprintf( buffer, "_appl_version, %04X\n", _appl_version );
		TAP_Print( buffer );
		TAP_Hdd_Fwrite( buffer, strlen(buffer), 1, fp );

		Output( _tap_startAddr );
		Output( __startPoint );
		Output( TAP_GetSystemProc );

		Output( TAP_SystemProc );
		Output( TAP_GetState );
		Output( TAP_ExitNormal );
		Output( TAP_EnterNormal );
		Output( TAP_Exit );

		Output( TAP_KbHit );
		Output( TAP_GetCh );
		Output( TAP_PutCh );
		Output( TAP_PutByte );
		Output( TAP_PrintSys );
		Output( TAP_Print );

		Output( TAP_SysOsdControl );
		Output( TAP_SetBaudRate );
		Output( TAP_SetBk );

		// -- COMMON FUNCTION ---------------------------------------------------

		Output( TAP_SetSoundLevel );
		Output( TAP_GetTime );
		Output( TAP_MemInfo );
		Output( TAP_MemAlloc );
		Output( TAP_MemFree );
		Output( TAP_GetTick );
		Output( TAP_Delay );
		Output( TAP_ExtractMjd );
		Output( TAP_MakeMjd );
		Output( TAP_Sin );
		Output( TAP_Cos );
		Output( TAP_SPrint );
		Output( TAP_GetSignalLevel );
		Output( TAP_GetSignalQuality );
		Output( TAP_SetInfoboxTime );
		Output( TAP_PlayPCM );
		Output( TAP_CaptureScreen );

		Output( TAP_SetSystemVar );
		Output( TAP_GetSystemVar );
		Output( TAP_WriteSystemVar );
		Output( TAP_GenerateEvent );

		// -- OSD FUNCTION ---------------------------------------------------
		Output( TAP_Osd_Create );
		Output( TAP_Osd_Delete );
		Output( TAP_Osd_Move );
		Output( TAP_Osd_FillBox );
		Output( TAP_Osd_PutBox );
		Output( TAP_Osd_GetBox );
		Output( TAP_Osd_SetLut );
		Output( TAP_Osd_Ctrl );
		Output( TAP_Osd_SetTransparency );

		Output( TAP_Osd_Draw3dBox );
		Output( TAP_Osd_Draw3dBoxFill );

		Output( TAP_Osd_DrawRectangle );
		Output( TAP_Osd_DrawPixmap );
		Output( TAP_Osd_Zoom );
		Output( TAP_Osd_GetBaseInfo );
		Output( TAP_Osd_Copy );

		Output( TAP_Osd_SaveBox );
		Output( TAP_Osd_RestoreBox );

		Output( TAP_Osd_GetPixel );
		Output( TAP_Osd_PutPixel );

		// -- HDD FUNCTION ---------------------------------------------------

		Output( TAP_Hdd_Fopen );
		Output( TAP_Hdd_Fread );
		Output( TAP_Hdd_Fwrite );
		Output( TAP_Hdd_Fclose );
		Output( TAP_Hdd_FindFirst );
		Output( TAP_Hdd_FindNext );
		Output( TAP_Hdd_Fseek );
		Output( TAP_Hdd_Flen );
		Output( TAP_Hdd_Ftell );
		Output( TAP_Hdd_TotalSize );
		Output( TAP_Hdd_FreeSize );
		Output( TAP_Hdd_Delete );
		Output( TAP_Hdd_Exist );
		Output( TAP_Hdd_Create );
		Output( TAP_Hdd_ChangeDir );

		Output( TAP_Hdd_PlayTs );
		Output( TAP_Hdd_StopTs );
		Output( TAP_Hdd_PlayMp3 );
		Output( TAP_Hdd_StopMp3 );
		Output( TAP_Hdd_Rename );

		Output( TAP_Hdd_StartRecord );
		Output( TAP_Hdd_StopRecord );
		Output( TAP_Hdd_GetRecInfo );
		Output( TAP_Hdd_GetPlayInfo );
		Output( TAP_Hdd_GetHddID );

		Output( TAP_Hdd_ChangePlaybackPos );
		Output( TAP_Hdd_GotoBookmark );
		Output( TAP_Hdd_SetBookmark );

		// -- STRING FUNCTION ---------------------
		Output( TAP_Osd_PutS );
		Output( TAP_Osd_GetW );
		Output( TAP_Osd_PutString );
		Output( TAP_Osd_DrawString );
		Output( TAP_Osd_PutStringAf );

		// -- WINDOW FUNCTION ---------------------------------------------------

		Output( TAP_Win_SetTitle );
		Output( TAP_Win_SetColor );
		Output( TAP_Win_SetDefaultColor );
		Output( TAP_Win_Draw );
		Output( TAP_Win_Create );
		Output( TAP_Win_Delete );
		Output( TAP_Win_SetFont );
		Output( TAP_Win_AddItem );
		Output( TAP_Win_GetSelection );
		Output( TAP_Win_SetSelection );
		Output( TAP_Win_Action );

		Output( TAP_Win_SetAvtice );
		Output( TAP_Win_SetDrawItemFunc );

		// ---------------------------------------------------------------------

		Output( TAP_Channel_GetTotalNum );
		Output( TAP_Channel_GetFirstInfo );
		Output( TAP_Channel_GetNextInfo );
		Output( TAP_Channel_GetInfo );
		Output( TAP_Channel_GetCurrent );
		Output( TAP_Channel_GetCurrentSubTV );
		Output( TAP_Channel_Start );
		Output( TAP_Channel_Scale );
		Output( TAP_Channel_IsPlayable );
		Output( TAP_Channel_Move );
		Output( TAP_Channel_Stop );
		Output( TAP_Channel_GetTotalAudioTrack );
		Output( TAP_Channel_GetAudioTrackName );
		Output( TAP_Channel_SetAudioTrack );
		Output( TAP_Channel_GetTotalSubtitleTrack );
		Output( TAP_Channel_GetSubtitleTrackName );
		Output( TAP_Channel_SetSubtitleTrack );
		Output( TAP_Channel_IsStarted );

		// --- EPG FUNCTIONS ---------------------------------------------------

		Output( TAP_GetEvent );
		Output( TAP_GetCurrentEvent );
		Output( TAP_PutEvent );

		Output( TAP_ControlEit );

		Output( TAP_EPG_UpdateEvent );
		Output( TAP_EPG_DeleteEvent );
		Output( TAP_EPG_GetExtInfo );

		// --- USB FUNCTIONS ---------------------------------------------------

		Output( TAP_Usb_Read );
		Output( TAP_Usb_Write );
		Output( TAP_Usb_PacketRead );
		Output( TAP_Usb_PacketWrite );
		Output( TAP_Usb_Cancel );

		// --- TIMER FUNCTIONS ---------------------------------------------------

		Output( TAP_Timer_GetTotalNum );
		Output( TAP_Timer_GetInfo );
		Output( TAP_Timer_Add );
		Output( TAP_Timer_Modify );
		Output( TAP_Timer_Delete );

		// --- VFD FUNCTIONS ---------------------------------------------------

		Output( TAP_Vfd_GetStatus );
		Output( TAP_Vfd_Control );
		Output( TAP_Vfd_SendData );
		
		TAP_Hdd_Fclose( fp );
	}

	return 0;
}
