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

#ifndef __TIMEREDITNEW__
  #define __TIMEREDITNEW__

#define __TIMEREDITNEW_VERSION__ "2008-01-16"

enum {
	TIMEREDITNEW_OK,
	TIMEREDITNEW_CANCEL
};

enum {
	TIMEREDITNEW_SECTIONTITLE,
	TIMEREDITNEW_TIMERNAME,
	TIMEREDITNEW_DATE,
	TIMEREDITNEW_TIMERSTART,
	TIMEREDITNEW_TIMEREND,
	TIMEREDITNEW_RESERVETYPE,
	TIMEREDITNEW_TIMERSELECTION,
	TIMEREDITNEW_OPTIONS
};

#define TIMEREDITNEW_PADDING DISPLAY_BORDER
#define TIMEREDITNEW_GAP DISPLAY_GAP

// Variables used for colours
static dword TimerEditNew_Fc = COLOR_White;
static dword TimerEditNew_HghBc = COLOR_Red;
static dword TimerEditNew_HghFc = COLOR_White;
static dword TimerEditNew_Bc = COLOR_Black;
static dword TimerEditNew_BaseColour = COLOR_Black;

static int TimerEditNew_Option_H = 30;
static int TimerEditNew_Option_W = 200;
static TYPE_TimerInfo TimerEditNew_Timer;
static bool TimerEditNew_Active = FALSE;
static word TimerEditNew_RGN = 0;
static word TimerEditNew_MemRGN = 0;
static byte* TimerEditNew_Base = NULL;
static bool TimerEditNew_Initalised = FALSE;
static int TimerEditNew_X = 0;
static int TimerEditNew_Y = 0;
static int TimerEditNew_W = 0;
static int TimerEditNew_H = 0;

static byte* TimerEditNew_ItemBase = NULL;
static byte* TimerEditNew_ItemHigh = NULL;
static byte* TimerEditNew_OkItemHigh = NULL;
static byte* TimerEditNew_CancelItemHigh = NULL;

static int TimerEditNew_Selected = 0;
static int TimerEditNew_PrevSelected = 0;
static int TimerEditNew_OptionSelected = 0;

void (*TimerEditNew_Callback)( int, bool, TYPE_TimerInfo );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerEditNew_IsActive( void );
void TimerEditNew_Init( word, word, int, int, int, int );
bool TimerEditNew_Activate( TYPE_TimerInfo, void (*ReturningProcedure)( int, bool, TYPE_TimerInfo ) );
void TimerEditNew_Deactivate( int, bool, TYPE_TimerInfo );
void TimerEditNew_DrawBackground( word );
dword TimerEditNew_KeyHandler( dword );
void TimerEditNew_SetColours( dword, dword, dword, dword, dword );
void TimerEditNew_DrawDetails( word );
void TimerEditNew_DrawItem( word, int, int );
void TimerEditNew_AlterStartTime( dword );
void TimerEditNew_AlterDuration( dword );
int TimerEditNew_Modify( void );
void TimerEditNew_KeyboardClose( char *, bool );
char * TimerEditNew_ReserveType( int );
void TimerEditNew_AdjustDate( dword );
void TimerEditNew_MemFree( void );
void TimerEditNew_CreateGradients( void );

#endif
