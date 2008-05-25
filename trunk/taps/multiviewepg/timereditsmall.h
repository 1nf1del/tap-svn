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

#ifndef __TIMEREDITSMALL__
  #define __TIMEREDITSMALL__

#define __TIMEREDITSMALL_VERSION__ "2008-01-16"

enum {
	TIMEREDITSMALL_OK,
	TIMEREDITSMALL_CANCEL
};

enum {
	TIMEREDITSMALL_SECTIONTITLE,
	TIMEREDITSMALL_TIMERNAME,
	TIMEREDITSMALL_DATE,
	TIMEREDITSMALL_TIMERSTART,
	TIMEREDITSMALL_TIMEREND,
	TIMEREDITSMALL_TIMERSELECTION,
	TIMEREDITSMALL_OPTIONS
};

#define TIMEREDITSMALL_PADDING DISPLAY_BORDER
#define TIMEREDITSMALL_GAP DISPLAY_GAP

// Variables used for colours
static dword TimerEditSmall_Fc = COLOR_White;
static dword TimerEditSmall_HghBc = COLOR_Red;
static dword TimerEditSmall_HghFc = COLOR_White;
static dword TimerEditSmall_Bc = COLOR_Black;
static dword TimerEditSmall_BaseColour = COLOR_Black;

static byte* TimerEditSmall_ItemBase = NULL;
static byte* TimerEditSmall_ItemHigh = NULL;
static byte* TimerEditSmall_OkItemHigh = NULL;
static byte* TimerEditSmall_CancelItemHigh = NULL;

static int TimerEditSmall_Option_H = 30;
static int TimerEditSmall_Option_W = 200;
static TYPE_TimerInfo TimerEditSmall_Timer;
static int TimerEditSmall_TimerNum = 0;
static bool TimerEditSmall_Active = FALSE;
static word TimerEditSmall_RGN = 0;
static word TimerEditSmall_MemRGN = 0;
static byte* TimerEditSmall_Base = NULL;
static bool TimerEditSmall_Initalised = FALSE;
static int TimerEditSmall_X = 0;
static int TimerEditSmall_Y = 0;
static int TimerEditSmall_W = 0;
static int TimerEditSmall_H = 0;

static int TimerEditSmall_Selected = 0;
static int TimerEditSmall_PrevSelected = 0;
static int TimerEditSmall_OptionSelected = 0;

void (*TimerEditSmall_Callback)( int, bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerEditSmall_IsActive( void );
void TimerEditSmall_Init( word, word, int, int, int, int );
bool TimerEditSmall_Activate( int, void (*ReturningProcedure)( int, bool ) );
void TimerEditSmall_Deactivate( int, bool );
void TimerEditSmall_DrawBackground( word );
dword TimerEditSmall_KeyHandler( dword );
void TimerEditSmall_SetColours( dword, dword, dword, dword, dword );
void TimerEditSmall_DrawDetails( word );
void TimerEditSmall_DrawItem( word, int, int );
void TimerEditSmall_AlterStartTime( dword );
void TimerEditSmall_AlterDuration( dword );
int TimerEditSmall_Modify( void );
bool TimerEditSmall_GetTimer( int );
void TimerEditSmall_KeyboardClose( char *, bool );
bool TimerEditSmall_ActivateWithTimer( TYPE_TimerInfo, int, void (*ReturningProcedure)( int, bool ) );
char * TimerEditSmall_ReserveType( int );
void TimerEditSmall_AdjustDate( dword );
void TimerEditSmall_MemFree( void );
void TimerEditSmall_CreateGradients( void );


#endif
