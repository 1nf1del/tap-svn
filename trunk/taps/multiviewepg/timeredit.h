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

#ifndef __TIMEREDIT__
  #define __TIMEREDIT__

#define __TIMEREDIT_VERSION__ "2008-01-16"

enum {
	TIMEREDIT_OK,
	TIMEREDIT_CANCEL
};

enum {
	TIMEREDIT_SECTIONTITLE,
	TIMEREDIT_TIMERNAME,
	TIMEREDIT_DATE,
	TIMEREDIT_TIMERSTART,
	TIMEREDIT_TIMEREND,
	TIMEREDIT_RESERVETYPE,
	TIMEREDIT_TIMERSELECTION,
	TIMEREDIT_OPTIONS
};

#define TIMEREDIT_PADDING DISPLAY_BORDER
#define TIMEREDIT_GAP DISPLAY_GAP

// Variables used for colours
static dword TimerEdit_Fc = COLOR_White;
static dword TimerEdit_HghBc = COLOR_Red;
static dword TimerEdit_HghFc = COLOR_White;
static dword TimerEdit_Bc = COLOR_Black;
static dword TimerEdit_BaseColour = COLOR_Black;

static int TimerEdit_Option_H = 30;
static int TimerEdit_Option_W = 200;
static TYPE_TimerInfo TimerEdit_Timer;
static int TimerEdit_TimerNum = 0;
static bool TimerEdit_Active = FALSE;
static word TimerEdit_RGN = 0;
static word TimerEdit_MemRGN = 0;
static byte* TimerEdit_Base = NULL;
static bool TimerEdit_Initalised = FALSE;
static int TimerEdit_X = 0;
static int TimerEdit_Y = 0;
static int TimerEdit_W = 0;
static int TimerEdit_H = 0;

static byte* TimerEdit_ItemBase = NULL;
static byte* TimerEdit_ItemHigh = NULL;
static byte* TimerEdit_OkItemHigh = NULL;
static byte* TimerEdit_CancelItemHigh = NULL;

static int TimerEdit_Selected = 0;
static int TimerEdit_PrevSelected = 0;
static int TimerEdit_OptionSelected = 0;

void (*TimerEdit_Callback)( int, bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerEdit_IsActive( void );
void TimerEdit_Init( word, word, int, int, int, int );
bool TimerEdit_Activate( int, void (*ReturningProcedure)( int, bool ) );
void TimerEdit_Deactivate( int, bool );
void TimerEdit_DrawBackground( word );
dword TimerEdit_KeyHandler( dword );
void TimerEdit_SetColours( dword, dword, dword, dword, dword );
void TimerEdit_DrawDetails( word );
void TimerEdit_DrawItem( word, int, int );
void TimerEdit_AlterStartTime( dword );
void TimerEdit_AlterDuration( dword );
int TimerEdit_Modify( void );
bool TimerEdit_GetTimer( int );
void TimerEdit_KeyboardClose( char *, bool );
bool TimerEdit_ActivateWithTimer( TYPE_TimerInfo, int, void (*ReturningProcedure)( int, bool ) );
char * TimerEdit_ReserveType( int );
void TimerEdit_AdjustDate( dword );
void TimerEdit_MemFree( void );
void TimerEdit_CreateGradients( void );

#endif
