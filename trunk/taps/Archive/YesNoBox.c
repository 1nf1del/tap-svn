/************************************************************
			Part of the ukEPG project
		This module is a generic Yes/No Box

Name	: YesNoBox.c
Author	: kidhazy
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 25-10-05  Inception date

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/

static char  YesNoOption;
static bool  returnFromYesNo;
static bool  yesnoWindowShowing;
static bool  YesNoResult;
static byte* YesNoWindowCopy;
static char  YesNoButton1[10];
static char  YesNoButton2[10];

void (*YesNoCallback)( bool );										// points to the procedure to be called when we've done


// Position of the Delete Confirmation Screen
#define YESNO_WINDOW_W 360
#define YESNO_WINDOW_H 180
#define YESNO_WINDOW_X ((MAX_SCREEN_X-YESNO_WINDOW_W)/2)   // Centre the yes/no window
#define YESNO_WINDOW_Y ((MAX_SCREEN_Y-YESNO_WINDOW_H)/2)   // Centre the yes/no window

#define YESNO_OPTION_W 102                                  // Width  of yes/no Options Buttons
#define YESNO_OPTION_H 43                                   // Height of yes/no Options Buttons
#define YESNO_OPTION_X  (YESNO_WINDOW_X + 52)              // Starting x-position for first Option button
#define YESNO_OPTION_Y  (YESNO_WINDOW_Y + YESNO_WINDOW_H - YESNO_OPTION_H - 15)             // Starting y-position for Option buttons.
#define YESNO_OPTION_X_SPACE   (YESNO_OPTION_W+50)         // Space between options on yes/no window.



//------------------------------------------------------------------
//
void DisplayYesNoLine(void)
{
	TAP_Osd_PutGd( rgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
    PrintCenter(rgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, YesNoButton1, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(rgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, YesNoButton2, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	switch ( YesNoOption )
	{

		case 0 :
					TAP_Osd_PutGd( rgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
				    PrintCenter(rgn, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(0*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, YesNoButton1, COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :
					TAP_Osd_PutGd( rgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y, &_bigkeygreenGd, FALSE );
					PrintCenter(rgn, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE), YESNO_OPTION_Y+8, YESNO_OPTION_X+(1*YESNO_OPTION_X_SPACE)+YESNO_OPTION_W, YesNoButton2, COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
	}
}



//-----------------------------------------------------------------------
//
void DisplayYesNoWindow(char* title, char* line1, char* line2, char* button1, char* button2, int defaultOption, void (*ReturningProcedure)( bool ) )
{
	YesNoCallback = ReturningProcedure;

    yesnoWindowShowing = TRUE;
    YesNoOption = defaultOption;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    YesNoWindowCopy = TAP_Osd_SaveBox(rgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H);

#ifdef WIN32  // If testing on WIN32 platform 
TAP_Osd_FillBox( rgn,YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H, FILL_COLOUR );				// clear the screen
#endif          

    // Display the pop-up window.
    TAP_Osd_PutGd( rgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, &_popup360x180Gd, TRUE );
	
    // Display Title and information in pop-up window
	PrintCenter( rgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  13, YESNO_WINDOW_X+YESNO_WINDOW_W-5, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	PrintCenter( rgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  56, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line1, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter( rgn, YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  89, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line2, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
    
    strcpy(YesNoButton1,button1);
    strcpy(YesNoButton2,button2);
    
    DisplayYesNoLine();
}


//-----------------------------------------------------------------------
//
void CloseYesNoWindow(void)
{
	yesnoWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H, YesNoWindowCopy);
	TAP_MemFree(YesNoWindowCopy);
	
	YesNoCallback( YesNoResult );
	
}

                                           
                                           
//------------
//
dword YesNoKeyHandler(dword key)
{

	switch ( key )
	{
		case RKEY_VolUp:	if ( YesNoOption < 1 ) YesNoOption++;
							else YesNoOption = 0;
							DisplayYesNoLine();
		     				break;

		case RKEY_VolDown:	if ( YesNoOption > 0 ) YesNoOption--;
							else YesNoOption = 1;
							DisplayYesNoLine();
							break;


		case RKEY_Ok :		
                            switch ( YesNoOption )
							{
								case 0 :   YesNoResult = TRUE;
                                           break;	// YES
                                           
								case 1 :   YesNoResult = FALSE;
                                           break;	// NO
                                           
								default :  YesNoResult = FALSE;
                                           break;
							}
							CloseYesNoWindow();		    // Close the yes/no window
                            returnFromYesNo = TRUE;		// will flag a redraw of file list
							break;


		case RKEY_Exit : 	
                            YesNoResult = FALSE;       // Default exit key to be FALSE.
                            CloseYesNoWindow();		   // Close the edit window
							returnFromYesNo = TRUE;	   // will cause a redraw of file list
							break;
							
		case RKEY_Mute :	return key;


		default :
			break;
	}
	return 0;
}



//------------
//
void initialiseYesNoWindow(void)
{
    yesnoWindowShowing = FALSE;
	YesNoOption        = 1;  // Default to the "NO" option.
	returnFromYesNo    = FALSE;
    YesNoResult        = FALSE;
}



