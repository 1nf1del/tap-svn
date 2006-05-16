/*
	Copyright (C) 2005-2006 Darkmatter

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

// Timer display, and management TAP
// This module is the main event handler

#include <tap.h>
#include <morekeys.h>

#define ID_ukEPG 0x800440FE

TAP_ID( ID_ukEPG );
TAP_PROGRAM_NAME("UkTimers v1.0");
TAP_AUTHOR_NAME("Darkmatter");
TAP_DESCRIPTION("Part of the UK TAP Project");
TAP_ETCINFO(__DATE__);

#include "Common.c"													// Global prototypes, graphics, and global variables
#include "logo.c"
#include "TimeBar.c"
#include "TimerDisplay.c"
#include "TimerEdit.c"
#include "TimerSaveRestore.c"
#include "MainMenu.c"
#include "ConfigMenu.c"
#include "IniFile.c"


static dword lastTick;
static byte oldMin;



//------------
//
void ActivationRoutine( void )
{
  	TAP_ExitNormal();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

	ActivateTimerWindow();	
}


void ExitRoutine( void )
{
    CloseTimerWindow();
    TAP_Osd_Delete( rgn );
	TAP_EnterNormal();
}


//------------
//
void TerminateRoutine( void )											// Performs the clean-up and terminate TAP
{
	TerminateMenu();
	TerminateConfigMenu();
	ReleaseLogoMemory();												// clean-up logo routines
	ExitRoutine();
	TAP_Exit();															// exit
}

//------------
//
dword My_KeyHandler(dword key, dword param2)
{
	dword state, subState ;
																			// check for currently active windows
	if ( configWindowShowing ) { ConfigKeyHandler( key ); return 0; }
																			
	if ( creditsShowing ) { CreditsKeyHandler( key ); return 0; }
																			
	if ( SaveRestoreWindowShowing ) {SaveRestoreKeyHandler( key); return 0; }
	    
	if ( menuShowing ) { MainMenuKeyHandler( key ); return 0; }				// Menu

	if ( editWindowShowing ) { TimerEditKeyHandler( key ); return 0; }		// timer edit

	if ( windowShowing ) { TimerWindowKeyHandler( key ); return 0; }		// timer display


    TAP_GetState(&state, &subState);
	if ((state != STATE_Normal) || (subState != SUBSTATE_Normal)) return key;	// otherwise just return if another menu is shown

	if (TAP_Channel_IsStarted( 0 )) return key;							// check for PIP


	if ( key == mainActivationKey )										// only enter our code from the normal state
	{
		ActivationRoutine();
		return 0;
	}

	return  key;														// default
}


//--------------------------
//
void CheckFlags( void )
{
	if ( exitFlag == TRUE ) { ExitRoutine(); exitFlag = FALSE; return; }	// close window and enter normal state
	if ( terminateFlag == TRUE ) { TerminateRoutine(); return; }			// clean-up and terminate TAP


	if ( returnFromEdit == TRUE )											// Handle navigation up level
	{																		// redraw the underlying window
	    returnFromEdit = FALSE;

		if ( menuShowing ) { RedrawMainMenu(); return; }					// Menu
		if ( editWindowShowing ) { RedrawTimerEdit(); return; }				// Timer edit
		if ( windowShowing ) { RefreshTimerWindow(); return; }				// Timer display
	}
}

//--------------------------
//
dword My_IdleHandler(void)												// periodic background routines
{																		// must ensure they do not take much processing power
    dword currentTickTime;
	byte 	hour, min, sec;
	word 	mjd;	

	if ( !windowShowing ) return;										// don't do anything until we are activated
	
	currentTickTime = TAP_GetTick();									// only get the current tick time once
	if ( keyboardWindowShowing ) KeyboardCursorBlink( currentTickTime );

																		// Display the clock
	if ( menuShowing ) return;											// but, not intested if these windows are showing
	if ( channelListWindowShowing ) return;
	if ( calendarWindowShowing ) return;
	if ( keyboardWindowShowing ) return;

   	TAP_GetTime( &mjd, &hour, &min, &sec);

	if ( min != oldMin )
	{
	    oldMin = min;
		UpdateListClock();
	}

}


//-----------------------------------------------------------------------
//
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	dword returnKey;
	returnKey = param1;

	switch(event)
	{
		case EVT_IDLE :	My_IdleHandler();
						break;

		case EVT_KEY :	if ( param1 == 0 ) break;						// means another TAP as exited the normal state

						returnKey = My_KeyHandler(param1, param2);
						CheckFlags();
						break;

		default : 		break;
	}
	return (returnKey);
}


int TAP_Main (void)
{
	LoadConfiguration();
	CacheLogos();

	initialiseTimerWindow();
	initialiseTimerEdit();
	InitialiseSaveRestore();
	initialiseMenu();
	InitialiseConfigRoutines();

	oldMin = 100;
	exitFlag = FALSE;
	terminateFlag = FALSE;
	returnFromEdit = FALSE;

    return 1;
}
