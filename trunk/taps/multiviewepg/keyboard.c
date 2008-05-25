//
//	  MultiViewEPG jim16 Tue 1 Apr 12:12:00
//
//	  Copyright (C) 2008 Jim16
//
//	  This is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  The software is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this software; if not, write to the Free Software
//	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "keyboard.h"
#include "fontman.h"
#include "skin.h"

char* Keyboard_GetText( void ){
	return Keyboard_Text;
}

int Keyboard_GetPosition( void ){
	return Keyboard_TextPosition;
}

bool Keyboard_IsActive( void ){
	return Keyboard_Active;
}

void Keyboard_MemFree( void ){
	if (Keyboard_KeyBase) TAP_MemFree(Keyboard_KeyBase);
	if (Keyboard_KeyHigh) TAP_MemFree(Keyboard_KeyHigh);
	Keyboard_KeyBase = NULL;
	Keyboard_KeyHigh = NULL;
}

void Keyboard_CreateGradients( void ){
	word rgn;
	Keyboard_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, KEYBOARD_KEY_W-2, KEYBOARD_KEY_H-2, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, KEYBOARD_KEY_W-2, KEYBOARD_KEY_H-2, Settings_GradientFactor, DISPLAY_KEYBOARDITEM);
		Keyboard_KeyBase = TAP_Osd_SaveBox(rgn, 0, 0, KEYBOARD_KEY_W-2, KEYBOARD_KEY_H-2);
		createGradient(rgn, KEYBOARD_KEY_W-2, KEYBOARD_KEY_H-2, Settings_GradientFactor, DISPLAY_KEYBOARDITEMSELECTED);
		Keyboard_KeyHigh = TAP_Osd_SaveBox(rgn, 0, 0, KEYBOARD_KEY_W-2, KEYBOARD_KEY_H-2);
		TAP_Osd_Delete(rgn);
	}
}


// Function to initalize the keyboard - region and location to render
void Keyboard_Init( word rgn, word memrgn, int x, int y, int w, int h, char * title ){
	Keyboard_RGN = rgn;	// Store the region to render on
	Keyboard_MemRGN = memrgn;
	Keyboard_X = x;
	Keyboard_Y = y;
	Keyboard_W = w;
	Keyboard_H = h;
	memset( Keyboard_Title, '\0', sizeof(Keyboard_Title) );
	strcpy(Keyboard_Title, title);
	Keyboard_Initalised = TRUE;
	// Calculate the key width and heights
	KEYBOARD_KEY_W = (Keyboard_W-(KEYBOARD_BORDER*2))/KEYBOARD_COLS;
	KEYBOARD_KEY_H = (Keyboard_H-(KEYBOARD_BORDER*3))/(KEYBOARD_ROWS+3);	// 2 is for string display and the title of the keyboard
	Keyboard_CreateGradients();
}

// Function to reset the keyboard
void Keyboard_Reset( void ){
	Keyboard_RGN = 0;	// Store the region to render on
	Keyboard_X = 0;
	Keyboard_Y = 0;
	Keyboard_W = 0;
	Keyboard_H = 0;
	Keyboard_Initalised = FALSE;
	Keyboard_KeyBc = COLOR_White;
	Keyboard_KeyHighBc = COLOR_Red;
	Keyboard_KeyFc = COLOR_Black;
	Keyboard_KeyHighFc = COLOR_White;
	Keyboard_Bc = COLOR_Black;
	Keyboard_FieldBc = COLOR_Black;
	Keyboard_FieldFc = COLOR_White;
	KEYBOARD_KEY_W = 30;
	KEYBOARD_KEY_H = 30;
	Keyboard_MemFree();
}

// Function to set the colours of the keyboard
void Keyboard_SetColours( dword kbc, dword kfc, dword khbc, dword khfc, dword ffc, dword fbc, dword bc ){
	Keyboard_KeyBc = kbc;
	Keyboard_KeyHighBc = khbc;
	Keyboard_KeyFc = kfc;
	Keyboard_KeyHighFc = khfc;
	Keyboard_FieldBc = fbc;
	Keyboard_FieldFc = ffc;
	Keyboard_Bc = bc;
}

// Function to activate the keyboard. Returns false if the keyboard hasn't been initalised
bool Keyboard_Activate( char * text, void (*ReturningProcedure)( char*, bool ) ){
	if( !Keyboard_Initalised ) return FALSE;
	Keyboard_Callback = ReturningProcedure;
	memset( Keyboard_Text, '\0', sizeof(Keyboard_Text) );
	memset( Keyboard_TextOriginal, '\0', sizeof(Keyboard_TextOriginal) );
	Keyboard_Active = TRUE;
	Keyboard_TextPosition = 0;
	Keyboard_Selected = 0;
	Keyboard_Changed = FALSE;
	Keyboard_Charset = CAPS_CHARSET;
	strcpy( Keyboard_TextOriginal, text );
	strcpy( Keyboard_Text, text );
	Keyboard_TextPosition = strlen( Keyboard_Text );
	// Now render the keyboard and the keys
	Keyboard_DrawBackground(Keyboard_MemRGN);
	Keyboard_RedrawKeys(Keyboard_MemRGN);
	Keyboard_DrawString(Keyboard_MemRGN);
	TAP_Osd_Copy( Keyboard_MemRGN, Keyboard_RGN, Keyboard_X, Keyboard_Y, Keyboard_W, Keyboard_H, Keyboard_X, Keyboard_Y, FALSE);
	return TRUE;
}

void Keyboard_Deactivate( bool cancel ){
	Keyboard_Active = FALSE;
	Keyboard_Changed = FALSE;
	Keyboard_Selected = -1;
	Keyboard_Charset = CAPS_CHARSET;	// Default back to caps
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(Keyboard_RGN, Keyboard_X, Keyboard_Y, Keyboard_W, Keyboard_H, Keyboard_Base);
	TAP_MemFree(Keyboard_Base);
	Keyboard_Base = NULL;
	Keyboard_MemFree();
	if( strlen( Keyboard_TextOriginal ) == 0 ){
		Keyboard_Callback(  Keyboard_Text, TRUE );	// Start with nothing so can't be worse so say it was successful
	} else if( strlen( Keyboard_Text ) == 0 ){		// Failed as the text has no size
		Keyboard_Callback(  Keyboard_TextOriginal, FALSE );
	} else if( cancel ){
		memset( Keyboard_Text, '\0', sizeof(Keyboard_Text) );
		Keyboard_Callback(  Keyboard_TextOriginal, FALSE );
	} else {
		// We get here by user not cancelling and both previous and current having some length so return true
		Keyboard_Callback(  Keyboard_Text, TRUE);
	}
}


// Function to draw the string into the box
void Keyboard_DrawString( word rgn ){
	char str[KEYBOARD_MAXLENGTH];
//	char test[128];
	int len;
	bool lettersLeft = FALSE;
	dword width = 0;
	int startPos = 0;
	int pad_y = 0;
	int fntSize = 0;
	// Bounds check
	len = strlen( Keyboard_Text );
	if ( len >= sizeof(str) ) return;
	memset( str, '\0', sizeof(str) );
	// Draw the text section
	TAP_Osd_FillBox(rgn, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H, Keyboard_W-(KEYBOARD_BORDER*2), KEYBOARD_KEY_H, Keyboard_FieldBc);
	// Copy the string
	strncpy( str, Keyboard_Text, Keyboard_TextPosition);		// copy up to the cursor
	str[ Keyboard_TextPosition ] = '\0';

	// Set the size of the text
	if( ( KEYBOARD_KEY_H - 4 ) > 19 && ( KEYBOARD_KEY_W - 4 ) > 19 ){
		fntSize = FNT_Size_1622;
		Keyboard_FntSize = 22;
		pad_y = (KEYBOARD_KEY_H/2)-(22/2);
	} else {
		pad_y = (KEYBOARD_KEY_H/2)-(19/2);
		fntSize = FNT_Size_1419;
		Keyboard_FntSize = 19;
	}
	
	width = TAP_Osd_GetW( str, 0, fntSize);
	
	while (width > (Keyboard_W-(5*KEYBOARD_BORDER)) ){	// String is too wide to fit on screen, so back off some letters.
		lettersLeft = TRUE;
		startPos++;
		strncpy( str, Keyboard_Text+startPos, Keyboard_TextPosition-startPos);
		str[ Keyboard_TextPosition-startPos ] = '\0';
		width = TAP_Osd_GetW( str, 0, fntSize);
	}
	
	switch( fntSize ){
		case FNT_Size_1419:
			// Render the title of the keyboard
			Font_Osd_PutString1419( rgn, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W, Keyboard_Title, Keyboard_KeyFc, COLOR_None );
			Font_Osd_PutString1419( rgn, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W-10, str, Keyboard_FieldFc, COLOR_None );
			break;
		case FNT_Size_1622:
			// Render the title of the keyboard
			Font_Osd_PutString1622( rgn, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W, Keyboard_Title, Keyboard_KeyFc, COLOR_None );
			Font_Osd_PutString1622( rgn, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W-10, str, Keyboard_FieldFc, COLOR_None );
			break;
		case FNT_Size_1926:
			// Render the title of the keyboard
			Font_Osd_PutString1622( rgn, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W, Keyboard_Title, Keyboard_KeyFc, COLOR_None );
			Font_Osd_PutString1926( rgn, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER+pad_y, Keyboard_X+Keyboard_W-10, str, Keyboard_FieldFc, COLOR_None );
			break;
	}
	Keyboard_CursorX = Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+width+3;
	
	// render << if letters left
	if( lettersLeft ){
		switch( fntSize ){
			case FNT_Size_1419:
				Font_Osd_PutString1419( rgn, Keyboard_X+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H+pad_y, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+KEYBOARD_BORDER+5, ":", Keyboard_FieldFc, COLOR_None );
				break;
			case FNT_Size_1622:
				Font_Osd_PutString1622( rgn, Keyboard_X+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H+pad_y, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+KEYBOARD_BORDER+5, ":", Keyboard_FieldFc, COLOR_None );
				break;
			case FNT_Size_1926:
				Font_Osd_PutString1926( rgn, Keyboard_X+KEYBOARD_BORDER+5, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H+pad_y, Keyboard_X+KEYBOARD_BORDER+KEYBOARD_BORDER+KEYBOARD_BORDER+5, ":", Keyboard_FieldFc, COLOR_None );
				break;
		}
	}
}

//
void Keyboard_CursorBlink( void ){
	static bool on = FALSE;
	static dword ticks = 0;
	if( !Keyboard_IsActive() ) return;
	if( ticks + 100 < TAP_GetTick() ){
		ticks = TAP_GetTick();
		if( on ){
			TAP_Osd_FillBox(Keyboard_RGN, Keyboard_CursorX+2, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER+((KEYBOARD_KEY_H/2)-(Keyboard_FntSize/2)), 2, KEYBOARD_KEY_H-(2*((KEYBOARD_KEY_H/2)-(Keyboard_FntSize/2))), Keyboard_FieldFc);
		} else {
			TAP_Osd_FillBox(Keyboard_RGN, Keyboard_CursorX+2, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER+((KEYBOARD_KEY_H/2)-(Keyboard_FntSize/2)), 2, KEYBOARD_KEY_H-(2*((KEYBOARD_KEY_H/2)-(Keyboard_FntSize/2))), Keyboard_FieldBc);
		}
		on = !on;
	}
}

// Very simple routine to append character to the end of the string
void Keyboard_AppendText( int row, int col ){
	char str[KEYBOARD_MAXLENGTH];
	char letter = Keyboard_GetChar( row, col );	// Get the selected character
	memset( str, '\0', sizeof(str) );
	if ( Keyboard_TextPosition > 0 ){	// We have some text to append to
		strncpy( str, Keyboard_Text, Keyboard_TextPosition);		// copy up to the cursor
	}
	str[ Keyboard_TextPosition ] = letter;										// Insert the new character
	Keyboard_TextPosition++;
	strcpy( Keyboard_Text, str );
	Keyboard_DrawString(Keyboard_MemRGN);
	TAP_Osd_Copy( Keyboard_MemRGN, Keyboard_RGN, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER, Keyboard_W-(KEYBOARD_BORDER*2), KEYBOARD_KEY_H, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H, FALSE);
	Keyboard_Changed = TRUE;
}

// Accessor
bool Keyboard_HasChanged( void ){
	return Keyboard_Changed;
}

// Modifier
void Keyboard_SetChanged( bool value ){
	Keyboard_Changed = value;
}

// Very simple routine to append character to the end of the string
void Keyboard_DeleteChar( void ){
	char str[KEYBOARD_MAXLENGTH];
	if( strlen(Keyboard_Text) > 0 && Keyboard_TextPosition > 0 ){	// Sanity check
		memset( str, '\0', sizeof(str) );
		strncpy( str, Keyboard_Text, Keyboard_TextPosition-1);
		Keyboard_TextPosition--;
		strcpy( Keyboard_Text, str );
		Keyboard_DrawString(Keyboard_MemRGN);
		TAP_Osd_Copy( Keyboard_MemRGN, Keyboard_RGN, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_KEY_H+KEYBOARD_BORDER, Keyboard_W-(KEYBOARD_BORDER*2), KEYBOARD_KEY_H, Keyboard_X+KEYBOARD_BORDER, Keyboard_Y+KEYBOARD_BORDER+KEYBOARD_KEY_H, FALSE);
		Keyboard_Changed = TRUE;
	}
}

// Keyhandler fo rthe keyboard
dword Keyboard_Keyhandler( dword param1 ){
	switch( param1 ){
		case RKEY_ChUp:
			Keyboard_PrevSelected = Keyboard_Selected;
			Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
			Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
			Keyboard_TempRow--;
			if( Keyboard_TempRow < 0 ) Keyboard_TempRow += KEYBOARD_ROWS;	// Check the range
			Keyboard_Selected = (Keyboard_TempRow*KEYBOARD_COLS)+Keyboard_TempCol;
			if( Keyboard_PrevSelected != Keyboard_Selected ){
				// Previous selection
				Keyboard_TempRow = (Keyboard_PrevSelected-(Keyboard_PrevSelected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_PrevSelected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);		
				// Current selection
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN,Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			} else {
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			}
			return 0;
			break;
		case RKEY_ChDown:
			Keyboard_PrevSelected = Keyboard_Selected;
			Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS; // Fine
			Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
			Keyboard_TempRow++;
			if( Keyboard_TempRow >= KEYBOARD_ROWS ) Keyboard_TempRow -= KEYBOARD_ROWS;	// Check the range
			Keyboard_Selected = (Keyboard_TempRow*KEYBOARD_COLS)+Keyboard_TempCol;
			if( Keyboard_PrevSelected != Keyboard_Selected ){
				// Previous selection
				Keyboard_TempRow = (Keyboard_PrevSelected-(Keyboard_PrevSelected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_PrevSelected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
				// Current selection
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			} else {
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			}
			return 0;
			break;
		case RKEY_VolDown:
			Keyboard_PrevSelected = Keyboard_Selected;
			// Calculate the position
			Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
			Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
			// Move the position
			Keyboard_TempCol--;
			if( Keyboard_TempCol < 0 ) Keyboard_TempCol += KEYBOARD_COLS;	// Check range
			Keyboard_Selected = (Keyboard_TempRow*KEYBOARD_COLS)+Keyboard_TempCol;
			if( Keyboard_PrevSelected != Keyboard_Selected ){
				// Previous selection
				Keyboard_TempRow = (Keyboard_PrevSelected-(Keyboard_PrevSelected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_PrevSelected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
				// Current selection
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			} else {
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			}
			return 0;
			break;
		case RKEY_VolUp:
			Keyboard_PrevSelected = Keyboard_Selected;
			// Calculate the position
			Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
			Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
			// Move the position
			Keyboard_TempCol++;
			if( Keyboard_TempCol >= KEYBOARD_COLS ) Keyboard_TempCol -= KEYBOARD_COLS;	// Check range
			Keyboard_Selected = (Keyboard_TempRow*KEYBOARD_COLS)+Keyboard_TempCol;
			if( Keyboard_PrevSelected != Keyboard_Selected ){
				// Previous selection
				Keyboard_TempRow = (Keyboard_PrevSelected-(Keyboard_PrevSelected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_PrevSelected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
				// Current selection
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			} else {
				Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
				Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
				Keyboard_DrawKey( Keyboard_RGN, Keyboard_TempRow, Keyboard_TempCol, Keyboard_Selected);
			}
			return 0;
			break;
		case RKEY_Ok:
			Keyboard_TempRow = (Keyboard_Selected-(Keyboard_Selected%KEYBOARD_COLS))/KEYBOARD_COLS;
			Keyboard_TempCol = Keyboard_Selected -(Keyboard_TempRow*KEYBOARD_COLS);
			Keyboard_AppendText( Keyboard_TempRow, Keyboard_TempCol );
			// TO DO ADD AUTOMATIC CHANGING OF CAPITALS TO LOWER CASE
//			Keyboard_Charset = LOWER_CHARSET;	// Reset to capitals as the user has added a space
//			Keyboard_RedrawKeys(Keyboard_RGN);
			return 0;
			break;
		case RKEY_Exit:
			Keyboard_Deactivate(TRUE);	// Say we canceled
			return 0;
			break;
		case RKEY_Record:
			Keyboard_Deactivate(FALSE);
			return 0;
			break;
		case RKEY_Forward:
			Keyboard_AppendText( KEYBOARD_ROWS, KEYBOARD_COLS );
			Keyboard_Charset = CAPS_CHARSET;	// Reset to capitals as the user has added a space
			Keyboard_RedrawKeys(Keyboard_RGN);
			return 0;
			break;
		case RKEY_Play:
			Keyboard_Charset++;
			if( Keyboard_Charset >= CHARSETS ){
				Keyboard_Charset = 0;
			}
			Keyboard_RedrawKeys(Keyboard_RGN);
			return 0;
			break;
		case RKEY_Rewind:
//			Keyboard_Charset--;
//			if( Keyboard_Charset < 0 ){
//				Keyboard_Charset = CHARSETS-1;
//			}
//			Keyboard_RedrawKeys(Keyboard_RGN);
			Keyboard_DeleteChar();
			return 0;
			break;
		case RKEY_Ab:
			Keyboard_DeleteChar();
			return 0;
			break;
	}
	return 0;
}

// Function to return the character at the row, col with repect to the Character set
char Keyboard_GetChar( int row, int col ){
	static char str[256];
	static int count = 0;
	switch (Keyboard_Charset){
		case CAPS_CHARSET:
			strcpy( str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ'\0" );
			break;
		case LOWER_CHARSET:
			strcpy( str, "abcdefghijklmnopqrstuvwxyz'\0" );
			break;
		case MISC_CHARSET:
			strcpy( str, "0123456789(|)^$!@&*-+,./?'\"\0" );
			break;
	}
	count = (row*7) + col;
	if( count < (strlen( str )) ){
		return str[count];
	} else {
		return ' ';
	}
}

// Function to draw keys - get row, col and selected
void Keyboard_DrawKey( word rgn, int row, int col, int selected ){
	int x, y, w, h;
	char c[10];
	int fntSize = FNT_Size_1419;
	int gap = 1;
	int pad_y = 0;
	int pad_x = 0;
	int char_w = 0;
	x = Keyboard_X + KEYBOARD_BORDER;// + ((Keyboard_W - (KEYBOARD_BORDER*2))+(((KEYBOARD_KEY_W+(gap*2))*KEYBOARD_COLS))/2);
	w = Keyboard_W - (KEYBOARD_BORDER*2);
	y = Keyboard_Y + KEYBOARD_KEY_H + KEYBOARD_KEY_H + (KEYBOARD_BORDER*2);
	h = Keyboard_H;
	// Sanity Check
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	
	// Set the size of the text
	if( ( KEYBOARD_KEY_H - 10 ) > 19 && ( KEYBOARD_KEY_W - 10 ) > 19 ){
//		if( ( KEYBOARD_KEY_H - 10 ) > 22 && ( KEYBOARD_KEY_W - 10 ) > 22 ){
//			// Use the big font
//			fntSize = FNT_Size_1926;
//			pad_y = (KEYBOARD_KEY_H/2)-(26/2);
//		} else {
			fntSize = FNT_Size_1622;
			pad_y = (KEYBOARD_KEY_H/2)-(22/2);
//		}
	} else {
		pad_y = (KEYBOARD_KEY_H/2)-(19/2);
	}
	
//	x += ((w+(KEYBOARD_BORDER*2)) - (KEYBOARD_COLS*(KEYBOARD_KEY_W+(gap*2))))/2;
	
	// Work out the padding for x for the character
	TAP_SPrint( c, "%c\0", Keyboard_GetChar(row,col)  );
	char_w = TAP_Osd_GetW(c,0,fntSize);
	pad_x = (KEYBOARD_KEY_W/2)-(char_w / 2);
	
	if( (row*7)+col == selected ){
		if( Keyboard_KeyHigh != NULL ){
			TAP_Osd_RestoreBox( rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyHigh );
		} else {
			TAP_Osd_FillBox(rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyHighBc);
		}
//		TAP_Osd_FillBox(rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyHighBc);
		switch( fntSize ){
			case FNT_Size_1419:
				Font_Osd_PutString1419( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyHighFc, COLOR_None);
				break;
			case FNT_Size_1622:
				Font_Osd_PutString1622( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyHighFc, COLOR_None);
				break;
			case FNT_Size_1926:
				Font_Osd_PutString1926( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyHighFc, COLOR_None);
				break;
		}
	} else {
		if( Keyboard_KeyBase != NULL ){
			TAP_Osd_RestoreBox( rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyBase );
		} else {
			TAP_Osd_FillBox(rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyBc);
		}
//		TAP_Osd_FillBox(rgn, x+(KEYBOARD_KEY_W*col)+gap, y+(KEYBOARD_KEY_H*row)+gap, KEYBOARD_KEY_W-(gap*2), KEYBOARD_KEY_H-(gap*2), Keyboard_KeyBc);
		switch( fntSize ){
			case FNT_Size_1419:
				Font_Osd_PutString1419( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyFc, COLOR_None);
				break;
			case FNT_Size_1622:
				Font_Osd_PutString1622( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyFc, COLOR_None);
				break;
			case FNT_Size_1926:
				Font_Osd_PutString1926( rgn, x+(KEYBOARD_KEY_W*col)+pad_x, y+(KEYBOARD_KEY_H*row)+pad_y, x+(KEYBOARD_KEY_W*col)+KEYBOARD_KEY_W, c, Keyboard_KeyFc, COLOR_None);
				break;
		}
	}
}

void Keyboard_DrawHelp( int rgn ){
	int x, y, w;//, h;
	//char c[10];
	int gap = 10;
	x = Keyboard_X + KEYBOARD_BORDER;// + ((Keyboard_W - (KEYBOARD_BORDER*2))+(((KEYBOARD_KEY_W+(gap*2))*KEYBOARD_COLS))/2);
	w = Keyboard_W - (KEYBOARD_BORDER*2);
	y = Keyboard_Y + KEYBOARD_KEY_H + KEYBOARD_KEY_H + (KEYBOARD_BORDER*2);
	
	TAP_Osd_PutGd( rgn, x, y+(KEYBOARD_KEY_H*4)+gap, &_rewindGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn,  x+30, y+(KEYBOARD_KEY_H*4)+gap, Keyboard_X-5+Keyboard_W-5, "Delete", DISPLAY_EVENT, COLOR_None);		
	
	TAP_Osd_PutGd( rgn, x+gap+80, y+(KEYBOARD_KEY_H*4)+gap, &_playGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn,  x+80+40, y+(KEYBOARD_KEY_H*4)+gap, Keyboard_X-5+Keyboard_W-5, "Case", DISPLAY_EVENT, COLOR_None);		

	TAP_Osd_PutGd( rgn, x+gap+80+80, y+(KEYBOARD_KEY_H*4)+gap, &_forwardGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn,  x+80+80+40, y+(KEYBOARD_KEY_H*4)+gap, Keyboard_X-5+Keyboard_W-5, "Space", DISPLAY_EVENT, COLOR_None);			
}

// Function to redraw all the keys
void Keyboard_RedrawKeys( word rgn ){
	int i,j;
	for( i = 0; i < KEYBOARD_ROWS; i++ ){
		for( j = 0; j < KEYBOARD_COLS; j++ ){
			Keyboard_DrawKey( rgn, i, j, Keyboard_Selected );
		}
	}
}

// Here we will just draw the basic layout of the keyboard
void Keyboard_DrawBackground( word rgn ){
	if( Keyboard_Base != NULL ) TAP_MemFree(Keyboard_Base);
	Keyboard_Base = TAP_Osd_SaveBox(Keyboard_RGN, Keyboard_X, Keyboard_Y, Keyboard_W, Keyboard_H);	// Save what is under thekeyboard
	TAP_Osd_FillBox(rgn, Keyboard_X, Keyboard_Y, Keyboard_W, Keyboard_H, Keyboard_Bc);
	Keyboard_DrawHelp(rgn);
}


