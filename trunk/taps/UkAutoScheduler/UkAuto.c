#ifdef WIN32
#include "TAP_Emulations.h"
#undef RGB
#define RGB(r,g,b)		( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
//#define RGB(r,g,b) ((COLORREF)(((BYTE)(r<<3)|((WORD)((BYTE)(g<<3))<<8))|(((DWORD)(BYTE)(b<<3))<<16)))
#endif           
                    
/************************************************************
				UkAuto
	Auto Scheduler TAP

		Part of the ukEPG project
	This module is the main event handler

Name	: UkAuto.c
Author	: sl8
Version	: 0.6
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.1 sl8: 11-11-05	Initial release
	  v0.2 sl8: 11-11-05 	Uses common UI (graphical design by Quixotic)
	  v0.3 sl8: 05-12-05 	Uses kidhazy's keyboard.
				Start/End time can now be entered directly via the number keys
				Will not highlight a line when number of schedules equal zero.
	  v0.4 sl8: 21-01-06	Uses Kidhazy's method of changing directories. Modified for TAP_SDK
				All variables initialised
				Save searchlist file when TAP exits.
	  v0.5 sl8: 06-02-06	Activiation key can now be changed
				Initialise schInitLcnToSvcNumMap from TAP_Main
	  v0.6 sl8: 07-02-06	Reload searchlist file each time TAP is activated

**************************************************************/

#define DEBUG 0

#include "tap.h"
#include "ukauto.h"

#define ID_UKAUTO 0x800440EE
#define TAP_NAME "UK Auto Scheduler"
#define VERSION "0.23x"

TAP_ID( ID_UKAUTO );


#if DEBUG
TAP_PROGRAM_NAME(TAP_NAME " v" VERSION "  -  " __TIME__ "  "__DATE__);
#else
TAP_PROGRAM_NAME(TAP_NAME " v" VERSION);
#endif

TAP_AUTHOR_NAME("sl8");
TAP_DESCRIPTION("Part of the UK TAP Project");
TAP_ETCINFO(__DATE__);

void ShowMessageWin(char*, char*);
static byte oldMin = 0;

#include "Common.c"
#include "Tools.c"
#include "logo.c"
#include "TextTools.c"
#include "schMain.c"
#include "schDisplay.c"
#include "schEdit.c"
#include "schFile.c"
#include "MainMenu.c"
#include "ConfigMenu.c"
#include "IniFile.c"

void ShowMessageWin (char* lpMessage, char* lpMessage1)
{
	int rgn_smw = 0;							// stores rgn-handle for osd
	dword w = 0;							// stores width of message-text

	rgn_smw = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );		// create rgn-handle
	w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;		// calculate width of message
	if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
	{
		w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;	// calculate width of message
	}
	if (w > 720) w = 720;						// if message is to long
	TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );	// draw background-box for border
	TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, lpMessage,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
	TAP_Osd_PutS(rgn, (720-w)/2, 270+26, (720+w)/2, lpMessage1,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,FALSE, ALIGN_CENTER);
	TAP_Delay(200);							// show it for 2 seconds
	TAP_Osd_Delete(rgn_smw);					// release rgn-handle
}


void ActivationRoutine( void )
{
	schInitRetreiveData();

  	TAP_ExitNormal();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

	ActivateDisplayWindow();	
}

void ExitRoutine( void )
{
	CloseTimerWindow();
	TAP_Osd_Delete( rgn );
	TAP_EnterNormal();
}


//------------
//
void TerminateRoutine( void )				// Performs the clean-up and terminate TAP
{
	if(schDisplaySaveToFile == TRUE)
	{
		schWriteSearchList();
	}

	TerminateMenu();
	TerminateConfigMenu();
	ReleaseLogoMemory();												// clean-up logo routines
	ExitRoutine();
	TAP_MemFree(schLcnToServiceTv);
	TAP_MemFree(schLcnToServiceRadio);

	if( schEpgData )
	{
		TAP_MemFree( schEpgData );
	}

	if( schEpgDataExtendedInfo )
	{
		TAP_MemFree( schEpgDataExtendedInfo );
	}

	TAP_Exit();															// exit
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

		if ( menuShowing ) { DisplayMenu( &mainMenu ); return; }		// Menu
		if ( schEditWindowShowing ) { RedrawSearchEdit(); return; }		// Timer edit
		if ( schDisplayWindowShowing ) { RefreshSearchWindow(); return; }	// Timer display
	}
}

dword My_KeyHandler(dword key, dword param2)
{
	dword state = 0, subState = 0;
																			
	if ( configWindowShowing ) { ConfigKeyHandler( key ); return 0; }

	if ( creditsShowing ) { CreditsKeyHandler( key ); return 0; }
																			
	if ( menuShowing ) { MainMenuKeyHandler( key ); return 0; }				// Menu

	if ( schEditWindowShowing ) { schEditKeyHandler( key ); return 0; }			// search edit

	if ( schDisplayWindowShowing ) { schDisplayKeyHandler( key ); return 0; }		// search display list

	TAP_GetState(&state, &subState);
	if ((state != STATE_Normal) || (subState != SUBSTATE_Normal)) return key;		// otherwise just return if another menu is shown

	if (TAP_Channel_IsStarted( 0 )) return key;						// check for PIP

	if
	(
		( key == mainActivationKey )
		&&
		( schServiceSV != SCH_SERVICE_INITIALISE )
	)
	{
		ActivationRoutine();

		return 0;
	}

	return  key;														// default
}

dword My_IdleHandler(void)
{
	dword	currentTickTime = 0;
	byte 	hour = 0, min = 0, sec = 0;
	word 	mjd = 0;

	currentTickTime = TAP_GetTick();				// only get the current tick time once
	if ( keyboardWindowShowing ) KeyboardCursorBlink( currentTickTime );

	if
	(
		(creditsShowing == FALSE)
		&&
		(menuShowing == FALSE)
		&&
		(schEditWindowShowing == FALSE)
		&&
		(schDisplayWindowShowing == FALSE)
	)
	{
		schService();
	}
	else
	{
   		TAP_GetTime( &mjd, &hour, &min, &sec);

		if ( min != oldMin )
		{
			oldMin = min;
//			UpdateListClock();
		}
	}
}

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	dword	returnKey = 0;

	returnKey = param1;

	switch(event)
	{
	case EVT_IDLE:
		My_IdleHandler();
		break;

	case EVT_KEY:
		if ( param1 != 0 )		// means another TAP as exited the normal state
		{
			returnKey = My_KeyHandler(param1, param2);
			CheckFlags();
		}
		break;
	default:
 		break;
	}

	return (returnKey);
}



int TAP_Main(void)
{
	FindTapDir();
	LoadConfiguration();
	CacheLogos();
	schInitLcnToSvcNumMap();
	initialiseTimerWindow();
	initialiseSearchEdit();
//	InitialiseSaveRestore();
	initialiseMenu();
	InitialiseConfigRoutines();

	oldMin = 100;
	exitFlag = FALSE;
	terminateFlag = FALSE;
	returnFromEdit = FALSE;

    return 1;
}

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
   return InitializeTAP_Environment(hInstance, nCmdShow, __tap_program_name__, TAP_Main, TAP_EventHandler);
} 
#endif
      
