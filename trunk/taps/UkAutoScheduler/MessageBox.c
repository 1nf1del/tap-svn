/************************************************************
			Part of the ukEPG project
		This module is a generic Message Box

Name	: MessageBox.c
Author	: sl8 (based on kidhazy's YesNoBox)
Version	: 0.1
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 sl8 14-12-06  Inception date

**************************************************************/

static bool  returnFromMessage;
static bool  MessageWindowShowing;
static bool  MessageResult;
static byte* MessageWindowCopy;

void (*MessageCallback)( bool );	

#define MESSAGE_WINDOW_W 360
#define MESSAGE_WINDOW_H 130
#define MESSAGE_WINDOW_X ((MAX_SCREEN_X-MESSAGE_WINDOW_W)/2)   // Centre the message window
#define MESSAGE_WINDOW_Y ((MAX_SCREEN_Y-MESSAGE_WINDOW_H)/2)   // Centre the message window

//-----------------------------------------------------------------------
//
void DisplayMessageWindow(char* title, char* line1, char* line2, void (*ReturningProcedure)( bool ) )
{
	MessageCallback = ReturningProcedure;

	MessageWindowShowing = TRUE;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
	MessageWindowCopy = TAP_Osd_SaveBox(rgn, MESSAGE_WINDOW_X, MESSAGE_WINDOW_Y, MESSAGE_WINDOW_W, MESSAGE_WINDOW_H);

	#ifdef WIN32  // If testing on WIN32 platform 
	TAP_Osd_FillBox( rgn,MESSAGE_WINDOW_X, MESSAGE_WINDOW_Y, MESSAGE_WINDOW_W, MESSAGE_WINDOW_H, FILL_COLOUR );				// clear the screen
	#endif          

	// Display the pop-up window.
	TAP_Osd_PutGd( rgn, MESSAGE_WINDOW_X, MESSAGE_WINDOW_Y, &_popup360x130Gd, TRUE );

	// Display Title and information in pop-up window
	PrintCenter( rgn, MESSAGE_WINDOW_X+5, MESSAGE_WINDOW_Y +  13, MESSAGE_WINDOW_X+MESSAGE_WINDOW_W-5, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	if(strlen(line2) == 0)
	{
		PrintCenter( rgn, MESSAGE_WINDOW_X+5, MESSAGE_WINDOW_Y +  72, MESSAGE_WINDOW_X+MESSAGE_WINDOW_W-5, line1, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	}
	else
	{
		PrintCenter( rgn, MESSAGE_WINDOW_X+5, MESSAGE_WINDOW_Y +  56, MESSAGE_WINDOW_X+MESSAGE_WINDOW_W-5, line1, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		PrintCenter( rgn, MESSAGE_WINDOW_X+5, MESSAGE_WINDOW_Y +  89, MESSAGE_WINDOW_X+MESSAGE_WINDOW_W-5, line2, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	}
}


//-----------------------------------------------------------------------
//
void CloseMessageWindow(void)
{
	MessageWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, MESSAGE_WINDOW_X, MESSAGE_WINDOW_Y, MESSAGE_WINDOW_W, MESSAGE_WINDOW_H, MessageWindowCopy);
	TAP_MemFree(MessageWindowCopy);
	
	MessageCallback( MessageResult );
}

                                           
                                           
//------------
//
dword MessageKeyHandler(dword key)
{
	switch ( key )
	{
	case RKEY_Ok :		
		CloseMessageWindow();
		returnFromMessage = TRUE;

		break;
	default:

		break;
	}

	return 0;
}



//------------
//
void initialiseMessageWindow(void)
{
	MessageWindowShowing = FALSE;
	returnFromMessage    = FALSE;
	MessageResult        = FALSE;
}



