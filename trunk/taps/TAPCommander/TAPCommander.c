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
#include "TAPExtensions.h"
#include "osd.h"


TAP_ID( 0x81001021 );
TAP_PROGRAM_NAME("TAP Commander 0.1 beta");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("List running TAPs");
TAP_ETCINFO(__DATE__);


static bool IsMainDialogShown = FALSE;			// stores state if main-dialog is shown
static TYPE_Window win;							// stores win-handle for main-dialog
static int rgn;									// stores rgn-handle for main-dialog


// array of event handler pointer of disabled TAPs
static TAP_EventHandlerFn disabledTAP[TAP_MAX];

// Generic handler substituted for other TAPs when they're disabled
// the real event handler address is stored in disabledTAP

dword DisabledTAP_EventHandler(word event, dword param1, dword param2 )
{
	// Do not be tempted do anything in this function.
	// accessing globals or string literals will cause the Toppy to crash
	// due to $gp having the wrong value for TAPCommander
	return param1;
}


void EnableTAP( int index, bool enable )
{
	if ( enable && disabledTAP[index] )
	{
		tapProcess[index].entryPoints->TAP_EventHandler = disabledTAP[index];
		disabledTAP[index] = NULL;
	}
	else if ( !enable && disabledTAP[index] == NULL )
	{
		disabledTAP[index] = tapProcess[index].entryPoints->TAP_EventHandler;
		tapProcess[index].entryPoints->TAP_EventHandler = DisabledTAP_EventHandler;
	}
}

bool IsEnabled( int index )
{
	return disabledTAP[index] == NULL;
}


void CreateMainDialog()
{
	int i;
	char str[200];
	TAPProcess* p;

	IsMainDialogShown = TRUE;

	TAP_Win_SetDefaultColor( &win );
	TAP_Win_Create( &win, rgn, 60, 60, 410, 385, FALSE, TRUE );
	TAP_Win_SetTitle( &win, "TAP Commander", 0, FNT_Size_1622 );

	for ( i = 0, p = tapProcess; i < TAP_MAX; ++i, ++p )
	{
		if ( p->entryPoints && p->header )
		{
			if ( p->header->id != __tap_ud__ )
			{
				// Must strcpy or a crash ensues
				sprintf( str, "%s%s", p->header->name, IsEnabled(i) ? "" : " (*)" );
				TAP_Win_AddItem( &win, str );
			}
		}
		else
		{
			strcpy( str, "(empty)");
			TAP_Win_AddItem( &win, str );
		}
	}

	TAP_Win_AddItem(&win,"Next TAP");
	TAP_Win_AddItem(&win,"Exit TAP");
}


void DestroyMainDialog()
{
	TAP_Win_Delete( &win );						// delete exit-menu win
	IsMainDialogShown = FALSE;					// erase flag
}


void ShowMainDialog()
{
	TAP_ExitNormal();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	CreateMainDialog();
}


void HideMainDialog()
{
	DestroyMainDialog();
	TAP_Osd_Delete( rgn );						// delete region-handle
	TAP_EnterNormal();
}


void ExitTAP()
{
	int i;

	TAP_Print( "Exiting TAP Commander\n" );
	for ( i = 0; i < TAP_MAX-1; ++i )
		EnableTAP( i, TRUE );
	TAP_Exit();
}


// Swap a pair of TAPs index upwards or downwards
bool SwapTaps( int offset, int index )
{
	int i;
	TAPProcess *process, *nextProcess;
	TAP_EventHandlerFn *fn, *nextFn;

	process = tapProcess;
	fn = disabledTAP;
	if ( process->header && process->header->id == __tap_ud__ )
	{
		++process;
		++fn;
	}
	nextProcess = process+1;
	nextFn = fn+1;
	for ( i = offset; i < TAP_MAX-2+offset; ++i, ++nextProcess, ++nextFn)
	{
		if ( nextProcess->header && nextProcess->header->id == __tap_ud__ )
		{
			++nextProcess;
			++nextFn;
		}
		if ( i == index )
		{
			TAPProcess temp;
			TAP_EventHandlerFn tempFn;
			memcpy( &temp, process, sizeof(TAPProcess) );
			memcpy( process, nextProcess, sizeof(TAPProcess) );
			memcpy( nextProcess, &temp, sizeof(TAPProcess) );
			tempFn = *fn;
			*fn = *nextFn;
			*nextFn = tempFn;
			return TRUE;
		}
		process = nextProcess;
		fn = nextFn;
	}

	return FALSE;
}


dword TAPCommanderProcessMainDialog( dword key )
{
	switch( key )
	{
	case RKEY_VolDown:
	{
		int selection = TAP_Win_GetSelection( &win );
		if ( selection > 0 && selection < TAP_MAX-1 )
		{
			// Move TAP up the list
			if ( SwapTaps( 1, selection ) )
			{
				DestroyMainDialog();
				CreateMainDialog();
				TAP_Win_SetSelection( &win, selection-1 );
			}
		}
		return 0;
	}
	case RKEY_VolUp:
	{
		int selection = TAP_Win_GetSelection( &win );
		if ( selection < TAP_MAX-1 )
		{
			// Move TAP down the list
			if ( SwapTaps( 0, selection ) )
			{
				DestroyMainDialog();
				CreateMainDialog();
				TAP_Win_SetSelection( &win, selection+1 );
			}
		}
		return 0;
	}

	// Red Configure
	case RKEY_F1:
	{
		int selection = TAP_Win_GetSelection( &win );
		int index = selection < *currentTAPIndex ? selection : selection+1;
		if ( index < TAP_MAX-1 )
		{
			TAP_SendEvent( index, EVT_TAPCONFIG, 0,0 );
		}
		return 0;
	}

	// Yellow Enable/Disable
	case RKEY_F3:
	{
		int selection = TAP_Win_GetSelection( &win );
		int index = selection < *currentTAPIndex ? selection : selection+1;
		if ( index < TAP_MAX-1 )
		{
			EnableTAP( index, !IsEnabled( index ) );
			DestroyMainDialog();
			CreateMainDialog();
			TAP_Win_SetSelection( &win, selection );
		}
		return 0;
	}

	// White Close
	case RKEY_Ab:
	{
		int selection = TAP_Win_GetSelection( &win );
		int index = selection < *currentTAPIndex ? selection : selection+1;
		if ( index < TAP_MAX-1 )
		{
			if ( TAP_SendEvent( index, EVT_TAPCLOSE, 0,0 ) )
			{
				DestroyMainDialog();
				CreateMainDialog();
				TAP_Win_SetSelection( &win, selection );
			}
		}
		return 0;
	}

	// OK Configure
	case RKEY_Ok:
	{
		int selection = TAP_Win_GetSelection( &win );
		int index = selection < *currentTAPIndex ? selection : selection+1;

		HideMainDialog();

		if ( index < TAP_MAX-1 )
		{
			TAP_SendEvent( index, EVT_TAPCONFIG, 0,0 );
		}
		else if ( selection == TAP_MAX-1 )
			return RKEY_Exit;
		else if ( selection == TAP_MAX )
		{
			ExitTAP();
		}
		return 0;
	}
	case RKEY_Exit:
		HideMainDialog();
		return 0;

	default:
		TAP_Win_Action( &win, key );				// send all other key's to menu-win
	}
	return 0;
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_KEY )
	{
		dword state, subState;

		if ( IsMainDialogShown == TRUE )
			return TAPCommanderProcessMainDialog( param1 );

		TAP_GetState( &state, &subState );
		if ( state == STATE_Normal && subState == SUBSTATE_Normal )
		{
			// Exit shows TAPCommander
			if ( param1 == RKEY_Exit && !OsdActive() )
			{
				ShowMainDialog();
				return 0;
			}
		}
		// Allow Recall to load TAPCommander from the main menu
		else if ( state == STATE_Menu && subState == SUBSTATE_MainMenu && param1 == RKEY_Recall )
		{
			ShowMainDialog();
			return 0;
		}
#ifdef DEBUG
		else if ( param1 == RKEY_Power )
		{
			ExitTAP();
			return 0;
		}
#endif
	}
	return param1;
}


int TAP_Main()
{
	// Initialise the firmware hack
	if ( !StartTAPExtensions() )
	{
		ShowMessage( "Cannot find TAP list address\n", 200 );
		return 0;
	}

	TAP_Print( "Starting TAP Commander\n" );

	return 1;
}
