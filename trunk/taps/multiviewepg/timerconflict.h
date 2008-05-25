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

#ifndef __TIMERCONFLICT__
  #define __TIMERCONFLICT__

#define __TIMERCONFLICT_VERSION__ "2008-01-15"

enum {
	TIMERCONFLICT_1,
	TIMERCONFLICT_2,
	TIMERCONFLICT_NEWTIMER,
	TIMERCONFLICT_TYPES
};

static word TimerConflict_RGN = 0;
static word TimerConflict_MemRGN = 0;
static int 	TimerConflict_X = 0;
static int TimerConflict_Y = 0;
static int TimerConflict_W = 0;
static int TimerConflict_H = 0;
static int TimerConflict_Option_H = 0;
static int TimerConflict_Option_W = 0;
static bool TimerConflict_Initalised = TRUE;
static byte *TimerConflict_Base = NULL;
static bool TimerConflict_Active = FALSE;
static int TimerConflict_Selected = 0;
static int TimerConflict_PrevSelected = 0;
static int TimerConflict_TimerNums = 0;
static bool TimerConflict_Redraw = FALSE;

static byte* TimerConflict_ItemBase = NULL;
static byte* TimerConflict_ItemHigh = NULL;

static TYPE_TimerInfo TimerConflict_1;
static TYPE_TimerInfo TimerConflict_2;
static int TimerConflict_1_Num;
static int TimerConflict_2_Num;
static TYPE_TimerInfo TimerConflict_SetAttempt;

#define TimerConflict_Label_H 30
#define TimerConflict_Help_H 40

void (*TimerConflict_Callback)( bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool TimerConflict_IsActive( void );
void TimerConflict_Init( word, word, int, int, int, int );
void TimerConflict_Activate( TYPE_TimerInfo, void (*ReturningProcedure)( bool ) );
void TimerConflict_Deactivate( bool );
void TimerConflict_RenderTimer( word, int, int );
void TimerConflict_DrawBackground( word );
void TimerConflict_Render( void );
bool TimerConflict_GetTimers( void );
dword TimerConflict_Keyhandler( dword );
void TimerConflict_TimerEditCallback( int, bool );
void TimerConflict_DeleteCallback( bool );
void TimerConflict_TimerEditNewCallback( int, bool, TYPE_TimerInfo );
void TimerConflict_MemFree( void );
void TimerConflict_CreateGradients( void );


#endif
