/************************************************************
				Part of the ukEPG project
			This module handles the menus

Name	: MainMenu.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	30-07-05	Split from menu.c as part of making the menu code generic

	Last change:  USE   1 Aug 105   11:17 am
************************************************************/

#include "Menu.c"

static menu_Struct	mainMenu;
static bool returnFromMenu = FALSE;
static bool returnFromCredits = FALSE;



//------------------
//
void DisplayCredits( void )
{
    char	str[256];
	creditsShowing = TRUE;
	returnFromCredits = FALSE;
        	
    DrawGraphicBorders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Credits", TITLE_COLOUR, COLOR_Black );

	TAP_Osd_PutStringAf1622( rgn, 58, 80, 650, __tap_program_name__ , MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 120, 650, "Written by Kidhazy (based on UK Timers code from Darkmatter)", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 150, 650, "Code contributions by IanP, DarkMatter, DB1, DX & Download", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 180, 650, "Graphical concept by Quixotic", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 270, 650, "Developed by the UK Project Team", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 300, 650, "Darkmatter, IanP, nwhitfield, richmac, sunstealer, Sulli, sgtwilko, matt", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 330, 650, "Traxmaster, garethm, rexy, chunkywizard, benhinman, tyreless", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 360, 650, "ando9185, sl8, bev, kidhazy, ROWANMOOR, BobD, simonc", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 500, 650, "Press EXIT to return to the menu", COLOR_Gray, 0 );
}


dword CreditsKeyHandler( dword key )
{
    if ( key == RKEY_Exit )
	{
		creditsShowing = FALSE;
	    returnFromCredits = TRUE;
	}

	return 0;
}


//------------------
// functions must tie up with text below
//
void ProcessMenuSelection( TYPE_MenuProcess menuProcess, int param1 )
{
	if ( menuProcess == MENU_Ok ) 										// user pressed the OK key has been pressed
	{
		switch ( param1 )												// menu line
		{
			case 1 :	CalledByTSRCommander=FALSE;
                        DisplayConfigWindow();
						break;

			case 2 :	terminateFlag = TRUE;							// Will cause TAP to terminate and unload.
					    break;											// No need to clean up here.

			case 3 :	DeletePlayData();
					    break;

			case 4 :	EmptyRecycleBin();
					    break;

			case 5 :	ClearLastViewFlags();
					    break;

			case 6 :	SetLastViewFlags();
					    break;

			case 7 :	DisplayCredits();
					    break;

			default:	break;
		}
		return;
	}

	if ( menuProcess == MENU_Exit )										// user pressed the EXIT key, or menu close for some other reason.
	{
		menuShowing = FALSE;
		returnFromMenu = TRUE;
	}
	
}



void initialiseMenu( void )
{
    menuShowing = FALSE;
	returnFromMenu = FALSE;
	returnFromCredits = FALSE;
    returnFromRecycleBinEmpty = FALSE;          // Set Recycle Bin flag here in case they go back into the Recycle Bin option.
	creditsShowing = FALSE;

	MenuCreate( &mainMenu, "Menu", &ProcessMenuSelection );
	MenuAdd( &mainMenu, "Configure", "Change the way this TAP works" );
	MenuAdd( &mainMenu, "Stop this TAP", "Terminate and unload this TAP" );
	MenuAdd( &mainMenu, "Delete All Progress Information", "Removes all playback progress information" );
	MenuAdd( &mainMenu, "Empty the Recycle Bin", "Removes all files in the recycle bin" );
	MenuAdd( &mainMenu, "Clear All 'New' Flags", "Removes all 'new' file indicators" );
	MenuAdd( &mainMenu, "Set All 'New' Flags TEST", "TESTING ONLY - delete lastview.dat" );
	MenuAdd( &mainMenu, "Credits & About Information", "Project Team & TAP Version information" );
}


void ActivateMenu(void)
{
	menuShowing = TRUE;
	returnFromMenu = FALSE;
	MenuSetCurrentLine( &mainMenu, 1 );									// default to the first item
	DisplayMenu( &mainMenu );
}


void RedrawMainMenu( void )
{
	DisplayMenu( &mainMenu );
}


void TerminateMenu( void )
{
	MenuDestroy( &mainMenu );
}


dword MainMenuKeyHandler( dword key )
{
	MenuKeyHandler( &mainMenu, key );
	return 0;
}
