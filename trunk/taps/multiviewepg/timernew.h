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

#ifndef __TIMERNEW__
  #define __TIMERNEW__

#define __TIMERNEW_VERSION__ "2008-01-16"

enum {
	TIMERNEW_OK,
	TIMERNEW_CANCEL
};

enum {
	TIMERNEW_SECTIONTITLE,
	TIMERNEW_TIMERNAME,
	TIMERNEW_RECORD,
	TIMERNEW_TYPE,
	TIMERNEW_CHANNEL,
	TIMERNEW_RESERVETYPE,
	TIMERNEW_DATE,
	TIMERNEW_TIMERSTART,
	TIMERNEW_TIMEREND,
	TIMERNEW_DURATION,
	TIMERNEW_TIMERSELECTION,
	TIMERNEW_OPTIONS
};

#define TIMERNEW_PADDING DISPLAY_BORDER
#define TIMERNEW_GAP DISPLAY_GAP

// Variables used for colours
static dword TimerNew_Fc = COLOR_White;
static dword TimerNew_HghBc = COLOR_Red;
static dword TimerNew_HghFc = COLOR_White;
static dword TimerNew_Bc = COLOR_Black;
static dword TimerNew_BaseColour = COLOR_Black;

static int TimerNew_Option_H = 30;
static int TimerNew_Option_W = 200;
static TYPE_TimerInfo TimerNew_Timer;
static bool TimerNew_Active = FALSE;
static word TimerNew_RGN = 0;
static word TimerNew_MemRGN = 0;
static byte* TimerNew_Base = NULL;
static bool TimerNew_Initalised = FALSE;
static int TimerNew_X = 0;
static int TimerNew_Y = 0;
static int TimerNew_W = 0;
static int TimerNew_H = 0;

static byte* TimerNew_ItemBase = NULL;
static byte* TimerNew_ItemHigh = NULL;
static byte* TimerNew_OkItemHigh = NULL;
static byte* TimerNew_CancelItemHigh = NULL;

static int TimerNew_Selected = 0;
static int TimerNew_PrevSelected = 0;
static int TimerNew_OptionSelected = 0;

void (*TimerNew_Callback)( int, bool, TYPE_TimerInfo );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerNew_IsActive( void );
void TimerNew_Init( word, word, int, int, int, int );
bool TimerNew_Activate( void (*ReturningProcedure)( int, bool, TYPE_TimerInfo ));
void TimerNew_Deactivate( int, bool );
void TimerNew_DrawBackground( word );
dword TimerNew_KeyHandler( dword );
void TimerNew_SetColours( dword, dword, dword, dword, dword );
void TimerNew_DrawDetails( word );
void TimerNew_DrawItem( word, int, int );
void TimerNew_AlterStartTime( dword );
void TimerNew_AlterDuration( dword );
int TimerNew_Add( void );
bool TimerNew_GetTimer( int );
void TimerNew_KeyboardClose( char *, bool );
char * TimerNew_ReserveType( int );
void TimerNew_AdjustDate( dword );
void TimerNew_MemFree( void );
void TimerNew_CreateGradients( void );
void TimerNew_ToggleRec( void );
void TimerNew_ToggleType( void );
void TimerNew_AlterChannel( dword key );

#endif
