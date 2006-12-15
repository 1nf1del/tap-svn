/************************************************************
				Part of the ukEPG project
			This module handles the menus

Name	: MainMenu.c
Author	: Darkmatter
Version	: 0.4
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	30-07-05	Split from menu.c as part of making the menu code generic
v0.1	sl8:	06-02-06	Config menu enabled
v0.2	sl8:	15-02-06	Credits modified.
v0.3	sl8:	11-04-06	Tidy up.
v0.4	sl8:	23-10-06	Credits modified.
v0.5	sl8:	15-12-06	Credits modified. Option to delete 'Already Recorded' data added.


************************************************************/

#include "Menu.c"

void returnFromDeleteEpisodeYesNo(bool);

static menu_Struct	mainMenu;



//------------------
//
void DisplayCredits( void )
{
	char	str[256];
	creditsShowing = TRUE;
	
	sysDrawGraphicBorders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Credits", TITLE_COLOUR, COLOR_Black );

	TAP_Osd_PutStringAf1622( rgn, 58, 80, 650, __tap_program_name__ , MAIN_TEXT_COLOUR, 0 );
	
	TAP_Osd_PutStringAf1622( rgn, 58, 120, 650, "Developed by the UK Project Team", MAIN_TEXT_COLOUR, 0 );
	
	TAP_Osd_PutStringAf1622( rgn, 58, 160, 650, "Written by sl8", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 190, 650, "Graphical concept by Quixotic", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 220, 650, "Code contributions by Darkmatter, IanP, kidhazy, sgtwilko, janilxx and Harvey", MAIN_TEXT_COLOUR, 0 );

	TAP_Osd_PutStringAf1622( rgn, 58, 270, 650, "Testing, and contibutions from the UK Project team:", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 300, 650, "Darkmatter, IanP, nwhitfield, richmac, sunstealer, Sulli, sgtwilko, matt", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 330, 650, "Traxmaster, garethm, rexy, chunkywizard, benhinman, tyreless", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 360, 650, "ando9185, sl8, bev, kidhazy, ROWANMOOR, awallin, BobD, DB1, DGB", MAIN_TEXT_COLOUR, 0 );
	TAP_Osd_PutStringAf1622( rgn, 58, 390, 650, "janilxx, rwg, simonc, tichtich, sjk, Jammer, Harvey", MAIN_TEXT_COLOUR, 0 );

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
	if ( menuProcess == MENU_Ok ) 						// user pressed the OK key has been pressed
	{
		switch ( param1 )						// menu line
		{
			case 1 :
					DisplayConfigWindow();
					    break;
			case 2 :	terminateFlag = TRUE;							// Will cause TAP to terminate and unload.
					    break;											// No need to clean up here.
			case 3 :	DisplayYesNoWindow("Delete Confirmation", "Delete all the episode information?", "", "Yes", "No", 1, &returnFromDeleteEpisodeYesNo );
					    break;
			case 4 :	DisplayCredits();
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
	MenuAdd( &mainMenu, "Stop this TAP", "Terminate and unload this TAP" );
	MenuAdd( &mainMenu, "Delete Episode Information", "Delete all episode information" );
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
	MenuDestory( &mainMenu );
}


dword MainMenuKeyHandler( dword key )
{
	MenuKeyHandler( &mainMenu, key );
	return 0;
}

void returnFromDeleteEpisodeYesNo( bool result)
{
	switch (result)
	{
	/* ---------------------------------------------------------------------------- */
	case TRUE:

		GotoPath(SETTINGS_FOLDER);
		if(TAP_Hdd_Exist(RECORDED_DB_DIR)) 
		{	
			TAP_Hdd_Delete(RECORDED_DB_DIR);

			TAP_Hdd_Create(RECORDED_DB_DIR,ATTR_FOLDER);
		}

		break;	// YES
	/* ---------------------------------------------------------------------------- */
	default:
	case FALSE:
	
		break;	// NO
	/* ---------------------------------------------------------------------------- */
	}   
}


