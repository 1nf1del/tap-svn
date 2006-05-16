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
// This module displays the keyboard. Permits the user to enter a string of characters


#include "graphics/BigKeyGreen.gd"
#include "graphics/BigKeyBlue.gd"


#define KB_BASE_X	122
#define KB_BASE_Y	70
#define KB_STEP_X	60
#define KB_STEP_Y	51
#define KB_COMMAND_STEP	105

#define KB_HIGHLIGHT_WIDTH 57
#define KB_WIDTH 400

#define KB_TOP_BACKGROUND RGB8888(90,104,213)
#define CURSOR_BLINK_TIME 50

typedef enum
{
	KB_Background,
	KB_Title,
	KB_Letter,
	KB_Control
} TYPE_KeyboardPosition;

//-------------------------------
// Prototypes (defined later)
void CloseKeyboard( bool );

//-------------------------------
// Global variables
static byte keyboardListType;
static dword lastCursorBlink;
static bool savedFlash;

static char currentString[256];
static char originalString[256];
static int currentRow;
static int currentColumn;
static int currentIndex;

void (*Callback)( char*, bool );										// points to the procedure to be called when we've done



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
    dword 	x_coord, y_coord, max, width;
	char	str[256];
	char	letter;
	int 	len, i;

   	len = strlen( currentString );
	if ( len >= sizeof(str) ) return;									// bounds check
	
	memset( str, '\0', sizeof(str) );									// much safer to capture any duff processing
	
	
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Title, 0, 0);
	max = x_coord + KB_WIDTH;


	if ( currentIndex > 0 )												// print up to the cursor
	{
		strncpy( str, currentString, currentIndex);
		str[ currentIndex ] = '\0';

		width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
		TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

		x_coord += width;
	}



	if ( ( len > 0 ) && ( currentIndex < len) )							// flash the character at the cursor
	{
		str[0] = currentString[currentIndex];							// manually create a 1 character string
		str[1] = '\0';
	}
	else TAP_SPrint( str, " " );

	width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
		
	if ( flashOn ) TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, x_coord+width, str, COLOR_Black, COLOR_White );
	else TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

	x_coord += width;



	if ( currentIndex < len )											// now split out everything to the right of the cursor
	{																	// (if anything)
		for ( i=0; i<len-currentIndex-1 ;i++ )
		{
		    str[i] = currentString[i+currentIndex+1];
		}
		str[i] = '\0';

		width = TAP_Osd_GetW( str, 0, FNT_Size_1926);
		TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, x_coord+width, str, MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );

		x_coord += width;
	}

	TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, max, " ", MAIN_TEXT_COLOUR, KB_TOP_BACKGROUND );
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
void AppendCharacter( char newCharacter )
{
	char	str[256];
	int		len, i;

   	len = strlen( currentString );
	if ( len >= sizeof(str) ) return;									// bounds check

	memset( str, '\0', sizeof(str) );									// much safer to capture any duff processing

	if ( currentIndex > 0 )	strncpy( str, currentString, currentIndex);		// copy up to the cursor
	str[ currentIndex ] = newCharacter;										// Insert the new character


	for ( i=currentIndex; i<len ;i++ )									// copy the rest of the string (if any)
	{
	    str[i+1] = currentString[i];
	}

	strcpy( currentString, str );
	currentIndex++;
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



//-----------------------------------------------
//
void KeyboardCursorBlink( dword currentTickTime )
{
	if ( (currentTickTime > (lastCursorBlink + CURSOR_BLINK_TIME)) || (currentTickTime < lastCursorBlink) )
	{
        savedFlash = !savedFlash;
		
		DisplayCurrentString( savedFlash );
		lastCursorBlink	= currentTickTime;
	}
}



//-----------------------------------------------
//
void KeyboardKeyHandler( dword key )
{

	
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
								AppendCharacter( KeyboardSelection( currentRow, currentColumn ) );
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
		case RKEY_9 :		AppendCharacter( key - RKEY_0 + '0' );						// pressing a number key inserts that character
							break;


		case RKEY_Recall :	strcpy( currentString, originalString );					// reload the original string
							currentIndex = strlen( currentString );

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

							
		case RKEY_Play :	AppendCharacter( ' ' );										// quick access key : Space
							break;
							
																						// quick access key : Shift (change list)
		case RKEY_TvRadio :	if ( keyboardListType < 3) keyboardListType++; else keyboardListType = 0;
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

	TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// Clear the screen
	
	CalcKeyboardPosition( &x_coord, &y_coord, KB_Background, row, column);
    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_calendarGd, TRUE );

	for ( column=0; column<7; column++)									// print the control buttons
	{
		HighlightLetter( 5, column, FALSE);
	}
																		// Display the letter in the current list
	RedrawList();
																		// Print the basic instructions (in center of screen)
	PrintCenter( rgn, 0, 503, 720, "<< and >> move the cursor", COLOR_DarkGray, 0, FNT_Size_1419 );
}



void ActivateKeyboard( char *str, void (*ReturningProcedure)( char*, bool ) )
{
	Callback = ReturningProcedure;

	keyboardWindowShowing = TRUE;
	keyboardListType = 0;
	currentRow = 5;
	currentColumn = 4;

	strcpy( originalString, str );
	strcpy( currentString, str );

	currentIndex = strlen( currentString );

	RedrawKeyboard();
}



void CloseKeyboard( bool successOrFail )
{
	keyboardWindowShowing = FALSE;
	returnFromEdit = TRUE;												// will cause a redraw of timer edit window
	Callback( currentString, successOrFail );
}



void InitialiseKeyboard( void )
{
	keyboardWindowShowing = FALSE;
	keyboardListType = 0;
	currentRow = 0;
	currentColumn = 0;
	lastCursorBlink = 0;
	savedFlash = FALSE;
}

