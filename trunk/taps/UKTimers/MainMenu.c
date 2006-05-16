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

// This module handles the menus


#include "Menu.c"

static menu_Struct	mainMenu;



//------------------
//
void DisplayCredits( void )
{
    char	str[256];
	creditsShowing = TRUE;
	
    DrawGraphicBoarders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Credits", TITLE_COLOUR, COLOR_Black );

	TAP_Osd_PutStringAf1622( rgn, 58, 80, 650, __tap_program_name__ , MAIN_TEXT_COLOUR, 0 );
	
	TAP_Osd_PutStringAf1622( rgn, 58, 120, 650, "Developed by the UK Project Team", MAIN_TEXT_COLOUR, 0 );
	
	TAP_Osd_PutStringAf1622( rgn, 58, 160, 650, "Written by Darkmatter", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 190, 650, "Graphical concept by Quixotic", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 220, 650, "Code contributions by IanP", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 270, 650, "Testing, and contibutions from the UK Project team:", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 300, 650, "Darkmatter, IanP, nwhitfield, richmac, sunstealer, Sulli, sgtwilko, matt", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 330, 650, "Traxmaster, garethm, rexy, chunkywizard, benhinman, tyreless", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 360, 650, "ando9185, sl8, bev, kidhazy, ROWANMOOR", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 500, 650, "Press EXIT to return to the menu", COLOR_DarkGray, 0 );
}


dword CreditsKeyHandler( dword key )
{
    if ( key == RKEY_Exit )
	{
		creditsShowing = FALSE;
		returnFromEdit = TRUE;
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
			case 1 :	DisplayConfigWindow();
						break;

			case 2 :	ExportTimers();
						break;

			case 3 : 	ImportTimers();
						break;

			case 4 : 	ImportFromDigiGuide();
						break;

			case 5 : 	DeleteAllTimers();
						break;

			case 6 :	terminateFlag = TRUE;							// Will cause TAP to terminate and unload.
					    break;											// No need to clean up here.

			case 7 :	DisplayCredits();
					    break;
																		// Terminate proc will take care of it all.
			default:	break;
		}
		return;
	}

	if ( menuProcess == MENU_Exit )										// user pressed the EXIT key, or menu close for some other reason.
	{
		menuShowing = FALSE;
		returnFromEdit = TRUE;											// will cause a redraw of timer list
	}
	
}



void initialiseMenu( void )
{
    menuShowing = FALSE;
	creditsShowing = FALSE;

	MenuCreate( &mainMenu, "Menu", &ProcessMenuSelection );
	MenuAdd( &mainMenu, "Configure", "Change the way this TAP works" );
	MenuAdd( &mainMenu, "Export Timers", "Save all timers to the TimerList.txt file" );
	MenuAdd( &mainMenu, "Import Timers", "Load timers from the TimerList.txt file" );
	MenuAdd( &mainMenu, "Import from DigiGuide", "Load timers from the ExportList.txt file" );
	MenuAdd( &mainMenu, "Delete all Timers", "Use with caution !" );
	MenuAdd( &mainMenu, "Stop this TAP", "Terminate and unload this TAP" );
	MenuAdd( &mainMenu, "Credits", "Thanks us if you like" );
}


void ActivateMenu(void)
{
	menuShowing = TRUE;
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
