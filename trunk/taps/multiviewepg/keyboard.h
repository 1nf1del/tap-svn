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

#ifndef __KEYBOARD__
  #define __KEYBOARD__

#define __KEYBOARD_VERSION__ "2008-01-15"

// Define charset types A,a,0 currently
enum {
	CAPS_CHARSET,
	LOWER_CHARSET,
	MISC_CHARSET,
	CHARSETS
};

// Define the number of rows and columns to use for keyboard
#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 7
#define KEYBOARD_GAP 5
#define KEYBOARD_BORDER 5
static int KEYBOARD_KEY_H = 30;
static int KEYBOARD_KEY_W = 30;

static int Keyboard_CursorX = 0;

// The variables used to determine where to draw keyboard
static int Keyboard_X = 0;
static int Keyboard_Y = 0;
static int Keyboard_W = 0;
static int Keyboard_H = 0;

// Variables used for colours
static dword Keyboard_KeyBc = COLOR_White;
static dword Keyboard_KeyHighBc = COLOR_Red;
static dword Keyboard_KeyFc = COLOR_Black;
static dword Keyboard_KeyHighFc = COLOR_White;
static dword Keyboard_Bc = COLOR_Black;
static dword Keyboard_FieldBc = COLOR_Black;
static dword Keyboard_FieldFc = COLOR_White;
static byte* Keyboard_Base = NULL;

static byte* Keyboard_KeyBase = NULL;
static byte* Keyboard_KeyHigh = NULL;

// Main variables
#define KEYBOARD_MAXLENGTH 96
static int Keyboard_TextPosition = 0;
static char Keyboard_TextOriginal[KEYBOARD_MAXLENGTH];
static char Keyboard_Text[KEYBOARD_MAXLENGTH];
static int Keyboard_Charset = CAPS_CHARSET;
static bool Keyboard_Active = FALSE;
static bool Keyboard_Changed = FALSE;
static bool Keyboard_Initalised = FALSE;	
static int Keyboard_Selected = -1;
static int Keyboard_PrevSelected = -1;
static word Keyboard_RGN = 0;
static word Keyboard_MemRGN = 0;
static int Keyboard_FntSize = 0;

static char Keyboard_Title[48];

// Variables used for calculating the move position
int Keyboard_TempRow = 0;
int Keyboard_TempCol = 0;

// Stores the callback routine for when the keyboard is closed
void (*Keyboard_Callback)( char*, bool );

// Define the functions used by the keyboard
bool Keyboard_IsActive( void );
void Keyboard_DrawBackground( word );
void Keyboard_RedrawKeys( word );
void Keyboard_DrawKey( word, int, int, int );
char Keyboard_GetChar( int, int );
dword Keyboard_Keyhandler( dword );
void Keyboard_DeleteChar( void );
void Keyboard_SetChanged( bool );
bool Keyboard_HasChanged( void );
void Keyboard_AppendText( int, int );
void Keyboard_DrawString( word );
void Keyboard_Deactivate( bool );
bool Keyboard_Activate( char *, void (*ReturningProcedure)( char*, bool ) );
void Keyboard_SetColours( dword, dword, dword, dword, dword, dword, dword );
void Keyboard_Reset( void );
void Keyboard_Init( word, word, int, int, int, int, char * );
char* Keyboard_GetText( void );
int Keyboard_GetPosition( void );
bool Keyboard_IsActive( void );
void Keyboard_CursorBlink( void );
void Keyboard_MemFree( void );
void Keyboard_CreateGradients( void );


#endif
