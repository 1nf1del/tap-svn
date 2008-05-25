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

#ifndef __TIMERLIST__
  #define __TIMERLIST__

#define __TIMERLIST_VERSION__ "2008-01-15"

#define TimerList_OPTIONS 12

enum {
	MON_TIMERS,
	TUE_TIMERS,
	WED_TIMERS,
	THU_TIMERS,
	FRI_TIMERS,
	SAT_TIMERS,
	SUN_TIMERS,
	ALL_TIMERS
};


static bool TimerList_Active = FALSE;
static int TimerList_Min = 0;
static int TimerList_Max = TimerList_OPTIONS;
static int TimerList_Selected = 0;
static int TimerList_PrevSelected = 0;
static bool TimerList_Redraw = FALSE;
static TYPE_TimerInfo TimerList_Timers[140];
static int TimerList_Timers_ptr = 0;
static int TimerList_ListingDay = ALL_TIMERS;
static int TimerList_TotalTimers = 0;
static byte* TimerList_ItemBase = NULL;
static byte* TimerList_ItemHigh = NULL;
static dword TimerList_LastRendered = 0;

void TimerList_Idle( void );
void TimerList_Render( void );
bool TimerList_IsActive( void );
void TimerList_Hide( void );
void TimerList_RenderDetails( void );
void TimerList_RenderItem( word, int, int, int, bool );
void TimerList_GetTimers( void );
void TimerList_Activate( void );
void TimerList_DeleteTimerCallback( bool );
dword TimerList_Keyhandler( dword );
int TimerList_FindFirstOccurance( TYPE_TimerInfo timer );
TYPE_TimerInfo TimerList_GetFirstOccurance( TYPE_TimerInfo timer );
void TimerList_MemFree( void );
void TimerList_CreateGradients( void );
void TimerList_RenderBlankItem( word rgn, int position );
void TimerList_RenderHelp( void );
void TimerList_NewTimerCallback( int i, bool cancel, TYPE_TimerInfo newtimer );
void TimerList_TimerConflictCallback( bool result );

#endif
