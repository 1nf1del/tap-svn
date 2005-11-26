/************************************************************
				Part of the ukEPG project
			This module displays the keyboard
		permits the user to enter a string of characters

Name	: Keyboard.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
Hisotry	: v0.0 Darkmatter: 11-08-05	Inception date. Constructed from calendar.c

	Last change:  USE  12 Aug 105    7:16 pm
**************************************************************/

/* Included in Common.c
#include "graphics/Calendar.GD"
#include "graphics/DateHighlight.GD"
#include "graphics/DateOverStamp.GD"


#include "graphics/BigKeyGreen.GD"
#include "graphics/BigKeyBlue.GD"
*/
#include "graphics/popup476x321.GD"
#include "graphics/popup520x321.GD"
#include "TIC.C"

#define KB_STEP_X	60
#define KB_STEP_Y	51
#define KB_COMMAND_STEP	105

#define KB_HIGHLIGHT_WIDTH 57
#define KB_WIDTH 476 // 400
#define KB_HEIGHT 416
#define KB_BASE_X	((MAX_SCREEN_X-KB_WIDTH)/2)
#define KB_BASE_Y	((MAX_SCREEN_Y-KB_HEIGHT)/2-10)
#define KB_TITLE_WIDTH 400
#define KB_TITLE_HEIGHT 26

#define KB_HELP_LINES1  11                          // Number of help lines in 1st column
#define KB_HELP_LINES2  11                          // Number of help lines in 2nd column
#define KB_HELP_HEIGHT 321
#define KB_HELP_WIDTH 520
#define KB_HELP_BASE_X  ((MAX_SCREEN_X-KB_HELP_WIDTH)/2)
#define KB_HELP_BASE_Y	((MAX_SCREEN_Y-KB_HELP_HEIGHT)/2)
#define KB_HELP_TEXT_X1  (KB_HELP_BASE_X + 10)
#define KB_HELP_TEXT_X2  (KB_HELP_BASE_X + (KB_HELP_WIDTH/2) + 40)
#define KB_HELP_TEXT_Y   (KB_HELP_BASE_Y + 65)
#define KB_HELP_TITLE_Y  (KB_HELP_BASE_Y + 15)
#define KB_HELP_KEY_W  80
#define KB_HELP_LINE_SPACING 19

#define KB_TOP_BACKGROUND RGB8888(90,104,213)
#define CURSOR_BLINK_TIME 50
#define REMOTE_INTERDIGIT_DELAY 300

#define INSERT_MODE    0
#define OVERWRITE_MODE 1


typedef enum
{
	KB_Background,
	KB_Title,
	KB_Letter,
	KB_Control,
	KB_Instructions
} TYPE_KeyboardPosition;

typedef enum
{
	Remote,
	Keyboard
} TYPE_EntryType;


//-------------------------------
// Prototypes (defined later)
void CloseKeyboard( bool );
void CloseKeyboardHelp( void );

//-------------------------------
// Global variables
static byte keyboardListType;
static dword lastCursorBlink;
static dword lastRemoteKeyDigit;
static bool savedFlash;

static char currentString[256];
static char originalString[256];
static int currentRow;
static int currentColumn;
static int currentIndex;
static byte* keyboardWindowCopy;
static byte* keyboardHelpWindowCopy;
static int glbMaxChars;  // Maximum number of characters that can be entered.
static int  insertMode;
static char insertModeStr[2][6] = {"[Ins]", "[Ovr]"};
static int  caseMode;
static char caseModeStr[3][6] = {"[Abc]", "[ABC]", "[abc]"};
//static char KeyCharArray[10][6] = {" &-'", "ABCD", "EFGH", "IJKL", "MNOP", "QRST", "UVWX", "YZ,.", "01234", "56789"};
static char KeyCharArray[10][15] = {" 0", ".,&-?'()[]+=!#", "ABC2", "DEF3", "GHI4", "JKL5", "MNO6", "PQRS7", "TUV8", "WXYZ9"};
static int  remoteKeyPressCount;                                        // Track how many times a remote control digit key is pressed.    
static int  remoteKeyPrevKey;                                           // Keep track of what was the last digit key pressed.
static bool remoteKeyFirstPress;                                        // Flag to indicate first time a key is pressed.
static bool remoteKeyActive;                                            // Is there a digit key character active awaiting timeout?
static word keyboardRgn;												// a memory region used for our text string.

void (*Callback)( char*, bool );										// points to the procedure to be called when we've done



//------------------------------------------------------------------
//
void DisplayKeyBoardHelp( void )
{
    char str[200];
    char key[22][15], help[22][30];
    
    int i, l;

	keyboardHelpWindowShowing = TRUE;
    
    TAP_SPrint( key[1] ,"Up/Down :");
    TAP_SPrint(help[1] ,"Move highlight Up/Down.");
    
    TAP_SPrint( key[2] ,"Left/Right :");
    TAP_SPrint(help[2] ,"Move highlight Left/Right.");
    
    TAP_SPrint( key[3] ,"<<   >> :");
    TAP_SPrint(help[3] ,"Move text cursor Left/Right.");
    
    TAP_SPrint( key[4] ,"OK :");
    TAP_SPrint(help[4] ,"Enter selected character.");
    
    TAP_SPrint( key[5] ,"Stop :");
    TAP_SPrint(help[5] ,"Change Insert/Overwrite mode.");
    
    TAP_SPrint( key[6] ,"Pause :");
    TAP_SPrint(help[6] ,"Change Upper/Lower Case mode.");
    
    TAP_SPrint( key[7] ,"White :");
    TAP_SPrint(help[7] ,"Delete a character.");
    
    TAP_SPrint( key[8] ,"Play :");
    TAP_SPrint(help[8] ,"Insert a space.");
    
    TAP_SPrint( key[9] ,"List :");
    TAP_SPrint(help[9] ,"Shift character set.");
    
    TAP_SPrint( key[10],"Record :");
    TAP_SPrint(help[10],"Save and exit.");
    
    TAP_SPrint( key[11],"Exit :");
    TAP_SPrint(help[11],"Discard and exit.");
    
    TAP_SPrint( key[12],"Recall :");
    TAP_SPrint(help[12],"Reload original.");
    
    TAP_SPrint( key[13],"1 :");
    TAP_SPrint(help[13],".,&-?'()[]+=!#");
    
    TAP_SPrint( key[14],"2 :");
    TAP_SPrint(help[14],"ABC2");
    
    TAP_SPrint( key[15],"3 :");
    TAP_SPrint(help[15],"DEF3");
        
    TAP_SPrint( key[16],"4 :");
    TAP_SPrint(help[16],"GHI4");
        
    TAP_SPrint( key[17],"5 :");
    TAP_SPrint(help[17],"JKL5");
        
    TAP_SPrint( key[18],"6 :");
    TAP_SPrint(help[18],"MNO6");
        
    TAP_SPrint( key[19],"7 :");
    TAP_SPrint(help[19],"PQRS7");
        
    TAP_SPrint( key[20],"8 :");
    TAP_SPrint(help[20],"TUV8");
        
    TAP_SPrint( key[21],"9 :");
    TAP_SPrint(help[21],"WXYZ9");
        
    TAP_SPrint( key[22],"0 :");
    TAP_SPrint(help[22],"[space]0");
        
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    keyboardHelpWindowCopy = TAP_Osd_SaveBox(rgn, KB_HELP_BASE_X, KB_HELP_BASE_Y, KB_HELP_WIDTH, KB_HELP_HEIGHT);
#ifdef WIN32  // If testing on WIN32 platform 
TAP_Osd_FillBox( rgn,KB_HELP_BASE_X, KB_HELP_BASE_Y, KB_HELP_WIDTH, KB_HELP_HEIGHT, FILL_COLOUR );				// clear the screen
#endif          

    // Display the pop-up window.
    TAP_Osd_PutGd( rgn, KB_HELP_BASE_X, KB_HELP_BASE_Y, &_popup520x321Gd, TRUE );

    TAP_SPrint(str, "Keyboard Help");
	PrintCenter( rgn, KB_HELP_TEXT_X1, KB_HELP_TITLE_Y, KB_HELP_BASE_X+KB_HELP_WIDTH, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	 
    i = 1;
    
    // Display the 1st column of help.
    for (l = 0; l< KB_HELP_LINES1; l++)
    {     
    	PrintRight( rgn, KB_HELP_TEXT_X1,                  KB_HELP_TEXT_Y + (l * KB_HELP_LINE_SPACING), KB_HELP_TEXT_X1+KB_HELP_KEY_W, key[i] , HEADING_TEXT_COLOUR, 0, FNT_Size_1419 );
        PrintLeft ( rgn, KB_HELP_TEXT_X1+KB_HELP_KEY_W+10, KB_HELP_TEXT_Y + (l * KB_HELP_LINE_SPACING), KB_HELP_BASE_X+KB_HELP_WIDTH,  help[i], MAIN_TEXT_COLOUR,    0, FNT_Size_1419 );
        i++;
    }
                 
    // Display the 2nd column of help.
    for (l = 0; l< KB_HELP_LINES2; l++)
    {     
    	PrintRight( rgn, KB_HELP_TEXT_X2,                  KB_HELP_TEXT_Y + (l * KB_HELP_LINE_SPACING), KB_HELP_TEXT_X2+KB_HELP_KEY_W, key[i] , HEADING_TEXT_COLOUR, 0, FNT_Size_1419 );
        PrintLeft ( rgn, KB_HELP_TEXT_X2+KB_HELP_KEY_W+10, KB_HELP_TEXT_Y + (l * KB_HELP_LINE_SPACING), KB_HELP_BASE_X+KB_HELP_WIDTH,  help[i], MAIN_TEXT_COLOUR,    0, FNT_Size_1419 );
        i++;
    }
    
    // Print some instructions at the bottom.
    TAP_SPrint(str, "(Press EXIT or INFO      to close this help window)");
	PrintCenter( rgn, KB_HELP_TEXT_X1, KB_HELP_BASE_Y + KB_HELP_HEIGHT - 35, KB_HELP_BASE_X+KB_HELP_WIDTH, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
    TAP_Osd_PutGd( rgn, KB_HELP_TEXT_X1+207, KB_HELP_BASE_Y + KB_HELP_HEIGHT - 35+2, &_infooval38x19Gd, TRUE );

}

     
     





//------------------------------------------------------------------
//
void CalcKeyboardPosition( dword *x, dword *y, TYPE_KeyboardPosition control, byte row, byte column)
{
    switch( control )
	{

		case KB_Background :	*x = KB_BASE_X;
						 		*y = KB_BASE_Y;
						 		break;
	
	    case KB_Title :			*x = KB_BASE_X + 30;
								*y = KB_BASE_Y + 15;
								break;

		case KB_Letter :	 	*x = KB_BASE_X + 22 + (column * KB_STEP_X);
						 		*y = KB_BASE_Y + 71 + (row * KB_STEP_Y);
								break;

		case KB_Control :		*x = KB_BASE_X + 22 + (column * KB_COMMAND_STEP);
								*y = KB_BASE_Y + 85 + (5 * KB_STEP_Y);
								break;
						 
		case KB_Instructions :	*x = KB_BASE_X;
								*y = KB_BASE_Y + KB_HEIGHT - 28;
								break;
						 
		default :				*x = KB_BASE_X;								// something sensible in case it goes wrong!
								*y = KB_BASE_Y;
								break;
	}
}



char KeyboardSelection( int row, int column )
{
    char	str[256];
	int 	count;
	
	switch ( keyboardListType )												// set up the selections strings
	{																		// avoid windows characters of \/:*?"<>|
		case 0 :	strcpy( str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ); break;
		
		case 1 :	strcpy( str, "abcdefghijklmnopqrstuvwxyz" ); break;

		case 2 :	strcpy( str, "&!£$#%+-^~=(){}[]_.,;@'0123456789" ); break;

		case 3 :	strcpy( str, "©®¢¥§µ¶°¹²³¼½¾«»¤·÷±¨¡¦¬­¯´ª¸º¿" ); break;

		default :	break;
	}

	count = (row*7) + column;											// Calculate the offset of the requested character

	if ( count < (strlen( str )) ) return str[count];					// bounds check before returning the character
	else return ' ';

}



//-------------------------------------
//
void HighlightLetter( int row, int column, bool highlight)
{
    dword 	x_coord, y_coord;
	char	str[256];

	if ( row < 5 )																// normal letter
	{
		CalcKeyboardPosition( &x_coord, &y_coord, KB_Letter, row, column);

		TAP_SPrint( str, "%c", KeyboardSelection( row, column )  );

        if ((row ==4) && (column == 6)) // Case Mode
		   TAP_SPrint( str, "%s", caseModeStr[caseMode]);
        	 
        if ((row ==4) && (column == 5)) // Insert/Overwrite Mode
		   TAP_SPrint( str, "%s",insertModeStr[insertMode]);

		if ( highlight )
		{
		    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_datehighlightGd, TRUE );	// display the highlight
			PrintCenter( rgn, x_coord, y_coord+9, (x_coord + KB_HIGHLIGHT_WIDTH), str, COLOR_Yellow, 0, FNT_Size_1926 );	// re-write the date text
		}
		else
		{
		    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_dateoverstampGd, TRUE );
			PrintCenter( rgn, x_coord, y_coord+9, (x_coord + KB_HIGHLIGHT_WIDTH), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
		}
	}
	else																		// or bottom control keys
	{
		switch ( column )
		{
			case 0 :
			case 1 : TAP_SPrint( str, "Shift" );
					 CalcKeyboardPosition( &x_coord, &y_coord, KB_Control, row, 0);
					 break;

			case 2 :
			case 3 : TAP_SPrint( str, "Save" );
					 CalcKeyboardPosition( &x_coord, &y_coord, KB_Control, row, 1);
					 break;

			case 4 : TAP_SPrint( str, "Cancel" );
					 CalcKeyboardPosition( &x_coord, &y_coord, KB_Control, row, 2);
					 break;

			case 5 :
			case 6 : TAP_SPrint( str, "Del" );
					 CalcKeyboardPosition( &x_coord, &y_coord, KB_Control, row, 3);
					 break;

					 
			default : TAP_SPrint( str, "BAD" );
					  CalcKeyboardPosition( &x_coord, &y_coord, KB_Control, row, 10);
					  break;
		}

		if ( highlight )
		{
		    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_bigkeygreenGd, TRUE );		// display the highlight
			PrintCenter( rgn, x_coord, y_coord+9, (x_coord + 102), str, COLOR_Yellow, 0, FNT_Size_1926 );	// re-write the date text
		}
		else
		{
		    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_bigkeyblueGd, TRUE );
			PrintCenter( rgn, x_coord, y_coord+9, (x_coord + 102), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
		}
	}
}



void DisplayCurrentString( bool flashOn )
{
    dword 	x_coord, y_coord, max, width, totalWidth;
	char	str[256];
	char	letter;
	int 	len, i, startPos, endPos;
	bool    charactersToLeft, charactersToRight;

   	len = strlen( currentString );
	if ( len >= sizeof(str) ) return;									// bounds check
	
	memset( str, '\0', sizeof(str) );									// much safer to capture any duff processing
	
	charactersToLeft = FALSE;
	charactersToRight = FALSE;
	
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Title, 0, 0);
	max = x_coord + KB_TITLE_WIDTH;

    totalWidth = 0;
    startPos = 0;
	if ( currentIndex > 0 )												// print up to the cursor
	{
		strncpy( str, currentString, currentIndex);
		str[ currentIndex ] = '\0';

		width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
//		if (width > KB_TITLE_WIDTH) width = KB_TITLE_WIDTH;             // Bounds check (added by kidhazy 3 Nov 2005)
		while (width > KB_TITLE_WIDTH-30)                               // String is too wide to fit on screen, so back off some letters.
		{
	       charactersToLeft = TRUE;
		   startPos++;
           strncpy( str, currentString+startPos, currentIndex-startPos);
		   str[ currentIndex-startPos ] = '\0';
		   width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
        }
        if (charactersToLeft) 
		   TAP_Osd_PutStringAf1926( keyboardRgn, x_coord-20, y_coord, x_coord+width, "«", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
        else
		   TAP_Osd_PutStringAf1926( keyboardRgn, x_coord-20, y_coord, x_coord+width, "  ", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
		TAP_Osd_PutStringAf1926( keyboardRgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
        
		x_coord += width;
		totalWidth = totalWidth + width;
	}



	if ( ( len > 0 ) && ( currentIndex < len) )							// flash the character at the cursor
	{
		str[0] = currentString[currentIndex];							// manually create a 1 character string
		str[1] = '\0';
	}
	else TAP_SPrint( str, " " );

	width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
		
	if ( flashOn ) TAP_Osd_PutStringAf1926( keyboardRgn, x_coord, y_coord, x_coord+width, str, COLOR_Black, COLOR_White );
	else TAP_Osd_PutStringAf1926( keyboardRgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

	x_coord += width;
    totalWidth = totalWidth + width;



	if ( currentIndex < len )											// now split out everything to the right of the cursor
	{																	// (if anything)
//		for ( i=0; i<len-currentIndex-1 ;i++ )
//		{
//		    str[i] = currentString[i+currentIndex+1];
//		}
//		str[i] = '\0';
        strncpy( str, currentString+currentIndex+1, len-currentIndex);
        str[ len-currentIndex ] = '\0';
        endPos = strlen(str);
		width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
		while (((totalWidth+width) > KB_TITLE_WIDTH) && (endPos>=0))                              // String is too wide to fit on screen, so back off some letters.
		{
	       charactersToRight = TRUE;
		   endPos--;
           strncpy( str, currentString+currentIndex+1, endPos);
		   str[ endPos ] = '\0';
		   width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
        }
		TAP_Osd_PutStringAf1926( keyboardRgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

		x_coord += width;
	}

	TAP_Osd_PutStringAf1926( keyboardRgn, x_coord, y_coord, max, " ", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

    // Recalculate starting position of Title string
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Title, 0, 0);
	max = x_coord + KB_TITLE_WIDTH;

    if (charactersToRight) 
       TAP_Osd_PutStringAf1926( keyboardRgn, max, y_coord, max+20, "»", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
    else
	   TAP_Osd_PutStringAf1926( keyboardRgn, max, y_coord, max+20, "  ", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
	
	// Copy the string from memory buffer onto the screen.
    TAP_Osd_Copy( keyboardRgn, rgn, x_coord-20, y_coord, KB_TITLE_WIDTH+40, KB_TITLE_HEIGHT, x_coord-20, y_coord, FALSE );


}



//-------------------------------------
//
void RedrawList( void )
{
	int		row, column;

	for ( row=0; row<5; row++)
	{
		for ( column=0; column<7; column++)
		{
			HighlightLetter( row, column, FALSE);
		}
	}

	HighlightLetter( currentRow, currentColumn, TRUE);
}


//----------------------------------------------------
// Insert the currently highlighted character in to the string
// at the current cursor position
//
void AppendCharacter( char newCharacter, TYPE_EntryType entryType )
{
	char	str[256];
	int		len, i, tempInsertMode;
	bool    bLowerCase;

   	len = strlen( currentString );
	if (( len >= sizeof(str) ) || ( len >= glbMaxChars ))									// bounds check
	{
#ifdef WIN32  // If testing on WIN32 platform don't include sound playback.
#else
       TAP_PlayPCM( (void *)_ticData, _ticSize, FREQ_48K, NULL );   	
#endif
       remoteKeyActive = FALSE;  // Clear any number waiting flag.
       return;
    }   

	memset( str, '\0', sizeof(str) );									// much safer to capture any duff processing

    switch (caseMode)
    {
         case 0:  // word case, i.e. upper if at start of buffer or previous char is space, else lower
                  bLowerCase = FALSE;
                  if (currentIndex > 0)
                  {
                      if (currentString[currentIndex - 1] != 32)
                         bLowerCase = TRUE;
                  }
                  break;
         
         case 1: bLowerCase = FALSE; break;
         
         case 2: bLowerCase = TRUE; break;
    }

    if (bLowerCase)
    {
       if (newCharacter >= 65 && newCharacter <= 90)
          newCharacter += 32;
    }

	if ( currentIndex > 0 )	strncpy( str, currentString, currentIndex);		// copy up to the cursor
	str[ currentIndex ] = newCharacter;										// Insert the new character

    tempInsertMode = insertMode;

    // If the key was entered via the numberpad and it is not the first press of this key, then switch to OVERWRITE mode.
    if ((entryType == Remote)   && (!remoteKeyFirstPress)) tempInsertMode = OVERWRITE_MODE;  // If still waiting for remote digit key to clear, force overwrite mode.

    // If the letter was from the normal keyboard and there's still a key from the numberpad waiting, switch to OVERWRITE mode to clear it.
    if ((entryType == Keyboard) && (remoteKeyActive)) tempInsertMode = OVERWRITE_MODE;  // If still waiting for remote digit key to clear, force overwrite mode.

    if (entryType == Keyboard)    remoteKeyActive=FALSE;   // Flag that we've cleared any waiting numberpad keys.
    
	for ( i=currentIndex; i<len ;i++ )									// copy the rest of the string (if any)
	{
	    str[i+1] = currentString[i+tempInsertMode];   
	}

	strcpy( currentString, str );
	if (entryType == Keyboard) currentIndex++;
	DisplayCurrentString( TRUE );

}



void DeleteCharacter( void )
{
	char	str[256];
	int		len, i;

   	len = strlen( currentString );
	if ( len == 0 )	return;												// bounds check
	if ( len >= sizeof(str) ) return;
	if ( currentIndex == 0 ) return;

	memset( str, '\0', sizeof(str) );									// much safer to capture any duff processing

	strncpy( str, currentString, currentIndex-1);						// copy up to the cursor (earlier bounds check ensures there'e something before the cursor)

																		// don't copy whatever is at the current cursor position
	for ( i=currentIndex; i<len ;i++ )									// then copy the rest of the string (if any)
	{
	    str[i-1] = currentString[i];
	}

	strcpy( currentString, str );
	currentIndex--;
	DisplayCurrentString( TRUE );
}




void AppendRemoteKeyCharacter( int key )
{
    int keyindex, currInsertMode;
    char keychar;

    if (key != remoteKeyPrevKey)
    {
       // If this is not the very first time through this routine, move to the next character location.
       if ((remoteKeyPrevKey != 0) && (remoteKeyActive))  currentIndex++;
       remoteKeyPrevKey = key;     // Save this new key so we can check it later.
       remoteKeyFirstPress=TRUE;   // Flag that this is the first press of this key.
       remoteKeyPressCount=0;      // Reset the number of times this key has been pressed.
    }
    else
       remoteKeyFirstPress=FALSE;  // Flag that this key has been pressed before.
    
    
    keyindex = key - RKEY_0;
    keychar = KeyCharArray[keyindex][remoteKeyPressCount];

    remoteKeyActive=TRUE;        // Flag that we have pressed a numberpad key and it's waiting to be cleared.
    AppendCharacter( keychar , Remote);

    remoteKeyPressCount++;
    if (remoteKeyPressCount >= strlen (KeyCharArray[keyindex]))
       remoteKeyPressCount = 0;

    lastRemoteKeyDigit = TAP_GetTick();  // Update timer as we've pressed another key.
     
}

//-----------------------------------------------
//
void KeyboardCursorBlink( dword currentTickTime )
{
    if ( (currentTickTime > (lastRemoteKeyDigit + REMOTE_INTERDIGIT_DELAY)) && (remoteKeyActive) )
    {
        remoteKeyPrevKey = 0;  // Reset the previous remote digit key, so that we skip to the next character.
        remoteKeyActive = FALSE;
        currentIndex++;
        lastRemoteKeyDigit = currentTickTime;
    }         

    if ( (currentTickTime > (lastCursorBlink + CURSOR_BLINK_TIME)) || (currentTickTime < lastCursorBlink) )
	{
        savedFlash = !savedFlash;
		
		DisplayCurrentString( savedFlash );
		lastCursorBlink	= currentTickTime;
	}
}



//-----------------------------------------------
//
void KeyboardHelpKeyHandler( dword key )
{
	switch ( key )
	{
		case RKEY_Info:
        case RKEY_Exit :	CloseKeyboardHelp();									// quick access key : cancel and exit
							break;

		default :			break;
	}
}




//-----------------------------------------------
//
void KeyboardKeyHandler( dword key )
{
    if ( keyboardHelpWindowShowing ) { KeyboardHelpKeyHandler( key ); return; }				// handle help screen

    // Handle reseting the previous press remote digit key. 
	switch ( key )
	{

		case RKEY_Ok :		if ( currentRow != 5 )										// if highlight is on the main part of the list
		    				{                                                           // insert the current character
								if ((currentRow == 4) && (currentColumn == 5))          // Toggle the Insert Mode
							        break;
								if ((currentRow == 4) && (currentColumn == 6))          // Toggle the Case Mode
							        break;
//                                remoteKeyPrevKey = 0;  // Reset the previous remote digit key, so that we skip to the next character.
    //lastRemoteKeyDigit = TAP_GetTick();
    								break;
							}
							break;
							
		case RKEY_Recall :	
	    case RKEY_Forward :	
		case RKEY_Rewind :	
		case RKEY_White :
        case RKEY_Pause:	
		case RKEY_Play :	
                            remoteKeyPrevKey = 0;  // Reset the previous remote digit key, so that we skip to the next character.
                            remoteKeyActive=FALSE;
                            lastRemoteKeyDigit = TAP_GetTick();
                            break;
        
        default :			break;

	}
	
	switch ( key )
	{


		case RKEY_VolDown:	HighlightLetter( currentRow, currentColumn, FALSE);			// clear the old highlight
																						// calculate the new cursor position
							if ( currentRow != 5 )
							{
								if ( currentColumn > 0 ) currentColumn--; else currentColumn = 6;
							}
							else
							{
								switch ( currentColumn )
								{
									case 0 :
									case 1 : currentColumn = 6; break;

									case 2 :
									case 3 : currentColumn = 0; break;

									case 4 : currentColumn = 2; break;
									
									case 5 :
									case 6 : currentColumn = 4; break;

									default : currentColumn = 2; break;
								}
							}

							HighlightLetter( currentRow, currentColumn, TRUE);			// highlight this new letter
							break;

		
		case RKEY_VolUp:	HighlightLetter( currentRow, currentColumn, FALSE);			// opposite of above

							if ( currentRow != 5 )
							{
								if ( currentColumn < 6 ) currentColumn++; else currentColumn = 0;
							}
							else
							{
								switch ( currentColumn )
								{
									case 0 :
									case 1 : currentColumn = 2; break;

									case 2 :
									case 3 : currentColumn = 4; break;

									case 4 : currentColumn = 6; break;

									case 5 :
									case 6 : currentColumn = 0; break;

									default : currentColumn = 2; break;
								}
							}

							HighlightLetter( currentRow, currentColumn, TRUE);
							break;

							
		case RKEY_ChUp:		HighlightLetter( currentRow, currentColumn, FALSE);			// clear the old highlight

							if ( currentRow > 0 ) currentRow--; else currentRow = 5;	// calculate the new cursor position
							HighlightLetter( currentRow, currentColumn, TRUE);			// highlight this new letter
							break;

								
		case RKEY_ChDown:	HighlightLetter( currentRow, currentColumn, FALSE);			// opposite of above

							if ( currentRow < 5 ) currentRow++; else currentRow = 0;
							HighlightLetter( currentRow, currentColumn, TRUE);
							break;


		case RKEY_Ok :		if ( currentRow != 5 )										// if highlight is on the main part of the list
		    				{                                                           // insert the current character
								if ((currentRow == 4) && (currentColumn == 5))          // Toggle the Insert Mode
								{
                                    insertMode = (insertMode + 1)%2;						
		                            RedrawList();
							        break;
                                }    
								if ((currentRow == 4) && (currentColumn == 6))          // Toggle the Case Mode
								{
                                    caseMode = (caseMode + 1)%3;						
		                            RedrawList();
							        break;
                                }    
								
                                AppendCharacter( KeyboardSelection( currentRow, currentColumn ) , Keyboard);
								break;
							}
							else														// must be on the control keys
							{
								switch ( currentColumn )
								{
									case 0 :                                            // SHIFT
									case 1 : if ( keyboardListType < 3) keyboardListType++; else keyboardListType = 0;
											 RedrawList();
											 break;

									case 2 :
									case 3 : CloseKeyboard( TRUE ); break;				// SAVE

									case 4 : CloseKeyboard( FALSE ); break;				// CANCEL

									case 5 :
									case 6 : DeleteCharacter(); break;					// DELETE

									default : break;
								}
							}
							break;

		case RKEY_0 :
		case RKEY_1 :
		case RKEY_2 :
		case RKEY_3 :
		case RKEY_4 :
		case RKEY_5 :
		case RKEY_6 :
		case RKEY_7 :
		case RKEY_8 :
		case RKEY_9 :		//AppendCharacter( key - RKEY_0 + '0' );						// pressing a number key inserts that character
		                    AppendRemoteKeyCharacter( key );						// pressing a number key inserts that character
							break;


		case RKEY_Recall :	strcpy( currentString, originalString );					// reload the original string
							currentIndex = strlen( currentString );
							break;
							
		case RKEY_Info :	// Display Keyboard Help.
                            DisplayKeyBoardHelp();
							break;
							
																						// cursor right
	    case RKEY_Forward :	if ( currentIndex < strlen( currentString )) currentIndex++;
							DisplayCurrentString( TRUE );
							break;
							

		case RKEY_Rewind :	if ( currentIndex > 0 ) currentIndex--;						// cursor left
							DisplayCurrentString( TRUE );
							break;


		case RKEY_White :	DeleteCharacter();											// quick access key : Delete
							break;

							
		case RKEY_Play :	AppendCharacter( ' ' , Keyboard );							// quick access key : Space
							break;
							
		case RKEY_Pause :	caseMode = (caseMode + 1)%3;								// quick access key : Change Case Mode
		                    RedrawList();
							break;
							
		case RKEY_Stop :	insertMode = (insertMode + 1)%2;							// quick access key : Change Insert Mode
		                    RedrawList();
							break;
							
																						// quick access key : Shift (change list)
		case RKEY_Filelist :	if ( keyboardListType < 3) keyboardListType++; else keyboardListType = 0;
							RedrawList();
							break;

							
		case RKEY_Exit :	CloseKeyboard( FALSE );										// quick access key : cancel and exit
							break;


		case RKEY_Record :	CloseKeyboard( TRUE );										// quick access key : save and exit

		default :			break;
	}
}



//-----------------------------------
//
void RedrawKeyboard( void )
{
    dword 	x_coord, y_coord;
	int		row, column, i;
	char	str[256];

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    keyboardWindowCopy = TAP_Osd_SaveBox(rgn, KB_BASE_X, KB_BASE_Y, KB_WIDTH, KB_HEIGHT);
#ifdef WIN32  // If testing on WIN32 platform 
TAP_Osd_FillBox( rgn,KB_BASE_X, KB_BASE_Y, KB_WIDTH, KB_HEIGHT, FILL_COLOUR );				// clear the screen
#endif          

    // Display the pop-up window.
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Background, row, column);
    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_popup476x416Gd, TRUE );

	for ( column=0; column<7; column++)									// print the control buttons
	{
		HighlightLetter( 5, column, FALSE);
	}
																		// Display the letter in the current list
	RedrawList();
				
	// Print the basic instructions (in center of screen)
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Instructions, row, column);
	PrintCenter( rgn, x_coord, y_coord, x_coord+KB_WIDTH, "(Press INFO         for Help Screen)", MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );
    TAP_Osd_PutGd( rgn, x_coord+199, y_coord, &_infooval38x19Gd, TRUE );
}



void ActivateKeyboard( char *str, int maxChars, void (*ReturningProcedure)( char*, bool ) )
{
	Callback = ReturningProcedure;
	glbMaxChars = maxChars;

	keyboardWindowShowing = TRUE;
	keyboardHelpWindowShowing = FALSE;
	keyboardListType = 0;
	currentRow = 5;
	currentColumn = 4;

	strcpy( originalString, str );
	strcpy( currentString, str );

	currentIndex = strlen( currentString );

	insertMode = 0;  // Start in insert mode.
	caseMode = 0; // Start in word Capital Mode (ie. Abcd Efg)
    remoteKeyPressCount=0;
    remoteKeyPrevKey=0;
    remoteKeyFirstPress=FALSE;
    remoteKeyActive=FALSE;

    keyboardRgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on
    // Display the pop-up window in the memory buffer so that we copy across any background.
//	CalcKeyboardPosition( &x_coord, &y_coord, KB_Background, row, column);
//    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_popup476x416Gd, TRUE );

	RedrawKeyboard();
}


void CloseKeyboardHelp( void )
{
	keyboardHelpWindowShowing = FALSE;
    lastRemoteKeyDigit = TAP_GetTick();   // Reset the remote key delay so that it text entry isn't updated straight away.
	TAP_Osd_RestoreBox(rgn, KB_HELP_BASE_X, KB_HELP_BASE_Y, KB_HELP_WIDTH, KB_HELP_HEIGHT, keyboardHelpWindowCopy);
	TAP_MemFree(keyboardHelpWindowCopy);
	
}


void CloseKeyboard( bool successOrFail )
{
	keyboardWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, KB_BASE_X, KB_BASE_Y, KB_WIDTH, KB_HEIGHT, keyboardWindowCopy);
	TAP_MemFree(keyboardWindowCopy);
	TAP_Osd_Delete(keyboardRgn);
	
	Callback( currentString, successOrFail );
}



void InitialiseKeyboard( void )
{
	keyboardWindowShowing = FALSE;
	keyboardHelpWindowShowing = FALSE;
	keyboardListType = 0;
	currentRow = 0;
	currentColumn = 0;
	lastCursorBlink = 0;
	savedFlash = FALSE;
	insertMode = 0;  // Start in insert mode.
	caseMode = 0; // Start in word Capital Mode (ie. Abcd Efg)
    remoteKeyPressCount=0;
    remoteKeyPrevKey=0;
    remoteKeyFirstPress=FALSE;
    remoteKeyActive=FALSE;
}

