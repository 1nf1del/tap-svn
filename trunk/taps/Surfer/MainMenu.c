/************************************************************
				Part of the UK TAP Project
			  This module handles the menus

Name	: MainMenu.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	02-08-05	Inception date
		  v0.1 Darkmatter

	Last change:  USE  16 Aug 105   12:16 pm
************************************************************/

#include "Menu.c"
#include "ConfigMenu.c"

static menu_Struct	mainMenu;
static bool menuShowing;
static bool creditsShowing;


//------------------
//
void DisplayCredits( void )
{
    char	str[256];
	creditsShowing = TRUE;
	
    DrawGraphicBoarders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Credits", TITLE_COLOUR, COLOR_Black );

	TAP_Osd_PutStringAf1622( rgn, 58, 80, 650, __tap_program_name__ , MAIN_TEXT_COLOUR, 0 );

 	TAP_Osd_PutStringAf1622( rgn, 58, 120, 650, "Written by Darkmatter & Kidhazy", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 150, 650, "Code contributions by IanP, Kidhazy, DB1 & DX", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 180, 650, "Graphical concept by Quixotic", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 270, 650, "Developed by the UK Project Team", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 300, 650, "Darkmatter, IanP, nwhitfield, richmac, sunstealer, Sulli, sgtwilko, matt", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 330, 650, "Traxmaster, garethm, rexy, chunkywizard, benhinman, tyreless", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 360, 650, "ando9185, sl8, bev, kidhazy, ROWANMOOR, BobD, simonc", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 500, 650, "Press EXIT to return to the menu", COLOR_DarkGray, 0 );
}


dword CreditsKeyHandler( dword key )
{
    if ( key == RKEY_Exit )
	{
		creditsShowing = FALSE;
		redrawWindow = TRUE;
	}

	return 0;
}


//------------------
// functions must tie up with text below
//
void ProcessMenuSelection( TYPE_MenuProcess menuProcess, int param1 )
{
	if ( menuProcess == MENU_Ok ) 										// user pressed the OK key
	{
		switch ( param1 )												// menu line
		{
			case 1 :	CalledByTSRCommander=FALSE;
                        DisplayConfigWindow();
						break;

			case 2 :	terminateFlag = TRUE;							// Will cause TAP to terminate and unload.
					    break;											// No need to clean up here.

			case 3 :	InitialiseScreenConfigWindow( OVERSCAN_WINDOW );      // EXPRESS Window settings.
					    break;
																	
			case 4 :	InitialiseScreenConfigWindow(1);      // ADVANCED Window settings.
					    break;
																	
			case 5 :	ResetScreenConfigWindow();            // Reset Window settings.
					    break;
																	
			case 6 :	DisplayCredits();
					    break;
																	
			default:	break;
		}
		return;
	}

	if ( menuProcess == MENU_Exit )										// user pressed the EXIT key, or menu closed for some other reason.
	{
		menuShowing = FALSE;
		redrawWindow = TRUE;											// will cause a redraw of Channel Info
	    if (!ScreenOn) SetTransparencyOnRegion( rgn );            // Hide the screen.
	}
	
}



void initialiseMenu( void )
{
    menuShowing = FALSE;
	creditsShowing = FALSE;
    if ((guideWindowDisplayed) && (GuideWindowOption == 1)) SwitchScreenToNormal();  // Reset Main Window if we previously shrunk it.
	MenuCreate( &mainMenu, "Menu", &ProcessMenuSelection );
	MenuAdd( &mainMenu, "Configure", "Change the way this TAP works" );
	MenuAdd( &mainMenu, "Stop this TAP", "Terminate and unload this TAP" );
	MenuAdd( &mainMenu, "Quick Window Configuration", "Quickly reposition or resize windows" );
	MenuAdd( &mainMenu, "Advanced Window Configuration", "Advanced reposition & resize of windows" );
	MenuAdd( &mainMenu, "Reset Window Sizes & Positions", "Reset default position & size of windows" );
 	MenuAdd( &mainMenu, "Credits & About Information", "Project Team & TAP Version information" );
}


void ActivateMenu(void)
{
	menuShowing = TRUE;
	MenuSetCurrentLine( &mainMenu, 1 );									// default to the first item
	SetTransparencyOffRegion( rgn );            // Make sure that the region is visible.
	DisplayMenu( &mainMenu );
}


void RedrawMainMenu( void )
{
	DisplayMenu( &mainMenu );
}


void TerminateMenu( void )
{
	MenuDestroy( &mainMenu );
    if ((guideWindowDisplayed) && (GuideWindowOption == 1)) SwitchScreenToGuideWindow();  // Reshrink Main Window if required.
}


dword MainMenuKeyHandler( dword key )
{
	if ( creditsShowing ) { CreditsKeyHandler( key ); return 0; }
	
	MenuKeyHandler( &mainMenu, key );
	return 0;
}
