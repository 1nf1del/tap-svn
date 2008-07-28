/*
	Copyright (C) 2007-2008 Simon Capewell

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
#include <tapconst.h>
#include <model.h>
#include "Settings.h"
#include "RemoteExtender.h"
#include "MHEGState.h"
#include "QuickAspectBlocker.h"


#define settingsFile "RemoteExtender.dat"
#define INI_VERSION 1

Settings settings;

static TYPE_Window window;
static dword rgn = 0;
static char* optionTextBuffer = 0;
static int optionCount = 3;

static void OptionsMenu_UpdateText();


// Add an item to the window
#define AddMenuItem(window, p) window.item[window.itemNum++] = p; p += strlen(p)+1

// Pad with spaces to as near a specific width as possible
static void PadToWidth( char* buffer, int padTo )
{
	int width = TAP_Osd_GetW( buffer, 0, FNT_Size_1622 );
	if ( width < padTo )
	{
		int spaceWidth = TAP_Osd_GetW( " ", 0, FNT_Size_1622 );
		char *end = buffer + strlen(buffer);
		int padCount = (padTo - width) / spaceWidth;
		if ( (padTo - width) % spaceWidth >= spaceWidth / 2 - 1 )
			padCount += 1;
		memset( end, ' ', padCount );
		end[padCount] = '\0';
	}
}

void OptionsMenu_Show()
{
	char buffer[256];

	if ( !rgn )
		rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, 0 );		// create rgn-handle

	TAP_Win_Delete( &window );

	TAP_ExitNormal();

	TAP_Win_SetDefaultColor( &window );
	TAP_Win_Create( &window, rgn, 220, 70, 280, 175, FALSE, FALSE );
	sprintf( buffer, "%s Options", __tap_program_name__ );
	TAP_Win_SetTitle( &window, buffer, 0, FNT_Size_1622 );

	// allocate memory for option text
	optionTextBuffer = (char*)malloc(0x40*(optionCount+5));
	OptionsMenu_UpdateText();
}


void OptionsMenu_Close()
{
	free(optionTextBuffer);
	optionTextBuffer = 0;
	TAP_Win_Delete( &window );
	TAP_Osd_Delete( rgn );
	rgn = 0;
	TAP_EnterNormal();
}


dword OptionsMenu_HandleKey( dword key, dword keyHW )
{
	int selection = TAP_Win_GetSelection( &window );

	switch ( key )
	{
	case RKEY_Ok:
		if ( selection == optionCount+1 )
		{
			// Cancel - Reload original settings
			OptionsMenu_Close();
			Settings_Load();
		}
		return 0;

	case RKEY_Recall:
		if ( selection == optionCount )
		{
			// Reset to defaults
			Settings_Reset();
			OptionsMenu_UpdateText();
		}
		return 0;

	case RKEY_Exit:
		OptionsMenu_Close();
		Settings_Save();
		UndoFirmwareHacks();
		RemoteExtender_Init();
		if ( settings.mheg )
			MHEGState_Enable();
		if ( settings.quickAspectBlocker )
			QuickAspectBlocker_Enable();
		return 0;

	case RKEY_VolUp:
		if ( selection == 0 && RemoteExtender_Available )
			settings.remoteExtendingDisabled = !settings.remoteExtendingDisabled;
		else if ( selection == 1 && MHEGState_Available )
			settings.mheg = !settings.mheg;
		else if ( selection == 2 && QuickAspectBlocker_Available )
			settings.quickAspectBlocker = !settings.quickAspectBlocker;
		else
			return 0;
		OptionsMenu_UpdateText();
		return 0;

	case RKEY_VolDown:
		if ( selection == 0 && MHEGState_Available )
			settings.mheg = !settings.mheg;
		else if ( selection == 1 && MHEGState_Available )
			settings.mheg = !settings.mheg;
		else if ( selection == 2 && QuickAspectBlocker_Available )
			settings.quickAspectBlocker = !settings.quickAspectBlocker;
		else
			return 0;
		OptionsMenu_UpdateText();
		return 0;

	case RKEY_ChDown:
		if ( selection < optionCount+1 )
		{
			TAP_Win_Action( &window, key );
			OptionsMenu_UpdateText();
		}
		return 0;
	case RKEY_ChUp:
		if ( selection > 0 )
		{
			TAP_Win_Action( &window, key );
			OptionsMenu_UpdateText();
		}
		return 0;

	default:
		TAP_Win_Action( &window, key ); // send all other key's to menu-Win
		return 0;
	}

	return key;
}


void OptionsMenu_UpdateText()
{
	int i;
	char* p;

	p = optionTextBuffer;
	window.itemNum = 0;

	strcpy( p, "Remote Extender" );
	PadToWidth( p, 220 );
	strcat( p, RemoteExtender_Available ? settings.remoteExtendingDisabled ? "Off" : "On" : "N/A" );
	AddMenuItem( window, p );

	strcpy( p, "MHEG Detection" );
	PadToWidth( p, 220 );
	strcat( p, MHEGState_Available ? settings.mheg ? "On" : "Off" : "N/A" );
	AddMenuItem( window, p );

	strcpy( p, "0 Switches Aspect Ratio" );
	PadToWidth( p, 220 );
	strcat( p, QuickAspectBlocker_Available ? settings.quickAspectBlocker ? "No" : "Yes" : "N/A");
	AddMenuItem( window, p );

	strcpy( p, "Reset to defaults" );
	AddMenuItem( window, p );

	strcpy( p, "Cancel" );
	AddMenuItem( window, p );

	*p = 0;
	AddMenuItem( window, p );

	// Display context sensitive help
	switch (TAP_Win_GetSelection( &window ))
	{
	case 0:
		strcpy( p, "Allows other TAPs to detect" );
		AddMenuItem( window, p );
		strcpy( p, "additional remote keys");
		AddMenuItem( window, p );
		break;
	case 1:
		strcpy( p, "Allows other TAPs to detect when" );
		AddMenuItem( window, p );
		strcpy( p, "interactive text is active");
		AddMenuItem( window, p );
		break;
	case 2:
		strcpy( p, "Allows you to turn off TV aspect" );
		AddMenuItem( window, p );
		strcpy( p, "ratio switching when 0 is pressed" );
		AddMenuItem( window, p );
		break;
	case 3:
		strcpy( p, "Press Recall to reset options to" );
		AddMenuItem( window, p );
		strcpy( p, "their defaults" );
		AddMenuItem( window, p );
		break;
	default:
		strcpy( p, " " );
		AddMenuItem( window, p );
		strcpy( p, " " );
		AddMenuItem( window, p );
		break;
	}

	// Force a redraw
	TAP_Win_SetSelection( &window, TAP_Win_GetSelection( &window ) );
}


bool OptionsMenu_IsActive()
{
	return optionTextBuffer != 0;
}


void Settings_Save()
{
	TYPE_File* fp;

	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( "ProgramFiles" );
	if ( !TAP_Hdd_Exist( "Settings" ) )
		TAP_Hdd_Create( "Settings", ATTR_FOLDER );
	TAP_Hdd_ChangeDir( "Settings" );

	if ( !TAP_Hdd_Exist( settingsFile ) )
		TAP_Hdd_Create( settingsFile, ATTR_NORMAL );

	if ( fp = TAP_Hdd_Fopen(settingsFile) )
	{
		int version = INI_VERSION;
		int zero[256];
		memset( zero, 0, sizeof(zero) );
		TAP_Hdd_Fwrite( &version, 1, sizeof(version), fp );
		TAP_Hdd_Fwrite( &settings, 1, sizeof(settings), fp );
		TAP_Hdd_Fwrite( zero, 1, sizeof(zero), fp );
		TAP_Hdd_Fclose( fp );
	}
}


void Settings_Load()
{
	TYPE_File* fp;
	bool validSettings = FALSE;

	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( "ProgramFiles" );
	TAP_Hdd_ChangeDir( "Settings" );

	if ( fp = TAP_Hdd_Fopen(settingsFile) )
	{
		int version = 0;
		TAP_Hdd_Fread( &version, 1, sizeof(version), fp );
		if ( version == INI_VERSION )
		{
			TAP_Hdd_Fread( &settings, 1, sizeof(settings), fp );
			validSettings = TRUE;
		}
		TAP_Hdd_Fclose( fp );
	}

	if ( !validSettings )
	{
		Settings_Reset();
		Settings_Save();
	}
}


void Settings_Reset()
{
	settings.remoteExtendingDisabled = FALSE;
	if ( GetModel() == TF5800 && GetModel() == TF5810t || GetModel() == TF5800t )
	{
		settings.mheg = TRUE;
	}
	else
	{
		settings.mheg = FALSE;
	}
	settings.quickAspectBlocker = FALSE;
}
