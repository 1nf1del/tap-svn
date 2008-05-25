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

#ifndef __RECORDINGEDIT__
  #define __RECORDINGEDIT__

#define __RECORDINGEDIT_VERSION__ "2008-01-16"

enum {
	RECORDINGEDIT_OK,
	RECORDINGEDIT_CANCEL
};

enum {
	RECORDINGEDIT_SECTIONTITLE,
	RECORDINGEDIT_TIMERSTART,
	RECORDINGEDIT_TIMEREND,
	RECORDINGEDIT_OPTIONS
};

#define RECORDINGEDIT_PADDING DISPLAY_BORDER
#define RECORDINGEDIT_GAP DISPLAY_GAP

// Variables used for colours
static dword RecordingEdit_Fc = COLOR_White;
static dword RecordingEdit_HghBc = COLOR_Red;
static dword RecordingEdit_HghFc = COLOR_White;
static dword RecordingEdit_Bc = COLOR_Black;
static dword RecordingEdit_BaseColour = COLOR_Black;

static int RecordingEdit_Option_H = 30;
static int RecordingEdit_Option_W = 200;
static int RecordingEdit_Slot = 0;
static bool RecordingEdit_Active = FALSE;
static word RecordingEdit_RGN = 0;
static word RecordingEdit_MemRGN = 0;
static byte* RecordingEdit_Base = NULL;
static bool RecordingEdit_Initalised = FALSE;
static int RecordingEdit_X = 0;
static int RecordingEdit_Y = 0;
static int RecordingEdit_W = 0;
static int RecordingEdit_H = 0;

static byte* RecordingEdit_ItemBase = NULL;
static byte* RecordingEdit_ItemHigh = NULL;

static int RecordingEdit_Selected = 0;
static int RecordingEdit_PrevSelected = 0;

void (*RecordingEdit_Callback)( void );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool RecordingEdit_IsActive( void );
void RecordingEdit_Init( word, word, int, int, int, int );
bool RecordingEdit_Activate( int slot, void (*ReturningProcedure)( void ) );
void RecordingEdit_Deactivate( int, bool );
void RecordingEdit_DrawBackground( word );
dword RecordingEdit_KeyHandler( dword );
void RecordingEdit_SetColours( dword, dword, dword, dword, dword );
void RecordingEdit_DrawDetails( word );
void RecordingEdit_DrawItem( word, int, int );
void RecordingEdit_AlterDuration( dword );
void RecordingEdit_KeyboardClose( char *, bool );
void RecordingEdit_MemFree( void );
void RecordingEdit_CreateGradients( void );

#endif
