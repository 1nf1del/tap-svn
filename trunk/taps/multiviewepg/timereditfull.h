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

#ifndef __TIMEREDITFULL__
  #define __TIMEREDITFULL__

#define __TIMEREDITFULL_VERSION__ "2008-01-16"

enum {
	TIMEREDITFULL_OK,
	TIMEREDITFULL_CANCEL
};

enum {
	TIMEREDITFULL_SECTIONTITLE,
	TIMEREDITFULL_TIMERNAME,
	TIMEREDITFULL_RECORD,
	TIMEREDITFULL_TYPE,
	TIMEREDITFULL_CHANNEL,
	TIMEREDITFULL_RESERVETYPE,
	TIMEREDITFULL_DATE,
	TIMEREDITFULL_TIMERSTART,
	TIMEREDITFULL_TIMEREND,
	TIMEREDITFULL_DURATION,
	TIMEREDITFULL_TIMERSELECTION,
	TIMEREDITFULL_OPTIONS
};

#define TIMEREDITFULL_PADDING DISPLAY_BORDER
#define TIMEREDITFULL_GAP DISPLAY_GAP

// Variables used for colours
static dword TimerEditFull_Fc = COLOR_White;
static dword TimerEditFull_HghBc = COLOR_Red;
static dword TimerEditFull_HghFc = COLOR_White;
static dword TimerEditFull_Bc = COLOR_Black;
static dword TimerEditFull_BaseColour = COLOR_Black;

static int TimerEditFull_Option_H = 30;
static int TimerEditFull_Option_W = 200;
static TYPE_TimerInfo TimerEditFull_Timer;
static int TimerEditFull_TimerNum = 0;
static bool TimerEditFull_Active = FALSE;
static word TimerEditFull_RGN = 0;
static word TimerEditFull_MemRGN = 0;
static byte* TimerEditFull_Base = NULL;
static bool TimerEditFull_Initalised = FALSE;
static int TimerEditFull_X = 0;
static int TimerEditFull_Y = 0;
static int TimerEditFull_W = 0;
static int TimerEditFull_H = 0;

static byte* TimerEditFull_ItemBase = NULL;
static byte* TimerEditFull_ItemHigh = NULL;
static byte* TimerEditFull_OkItemHigh = NULL;
static byte* TimerEditFull_CancelItemHigh = NULL;

static int TimerEditFull_Selected = 0;
static int TimerEditFull_PrevSelected = 0;
static int TimerEditFull_OptionSelected = 0;

void (*TimerEditFull_Callback)( int, bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerEditFull_IsActive( void );
void TimerEditFull_Init( word, word, int, int, int, int );
bool TimerEditFull_Activate( int, void (*ReturningProcedure)( int, bool ) );
void TimerEditFull_Deactivate( int, bool );
void TimerEditFull_DrawBackground( word );
dword TimerEditFull_KeyHandler( dword );
void TimerEditFull_SetColours( dword, dword, dword, dword, dword );
void TimerEditFull_DrawDetails( word );
void TimerEditFull_DrawItem( word, int, int );
void TimerEditFull_AlterStartTime( dword );
void TimerEditFull_AlterDuration( dword );
int TimerEditFull_Modify( void );
bool TimerEditFull_GetTimer( int );
void TimerEditFull_KeyboardClose( char *, bool );
bool TimerEditFull_ActivateWithTimer( TYPE_TimerInfo, int, void (*ReturningProcedure)( int, bool ) );
char * TimerEditFull_ReserveType( int );
void TimerEditFull_AdjustDate( dword );
void TimerEditFull_MemFree( void );
void TimerEditFull_CreateGradients( void );
void TimerEditFull_ToggleRec( void );
void TimerEditFull_ToggleType( void );
void TimerEditFull_AlterChannel( dword key );

#endif
