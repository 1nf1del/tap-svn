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

#ifndef __TIMERSSTUFF__
  #define __TIMERSSTUFF__

#define __TIMERSSTUFF_VERSION__ "2008-01-15"

enum{
	TIMER_FULL,
	TIMER_PARTIAL,
	TIMER_NONE,
	TIMER_TYPES
};

enum {
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
	SUN
};

#define TIMERS_MAXDAYSAHEAD 7

int Timers_IsRecordingEvent( int );
int Timers_HasTimer( int );
int Timers_GetNeighbour( TYPE_TimerInfo );
char * Timers_GetTimerName( int );
char * Timers_GetNeighbourName( TYPE_TimerInfo );
int Timers_GetNeighbourWithNum( int, TYPE_TimerInfo );
char * Timers_BuildTimerName( TYPE_TimerInfo );
int Timers_GetNumber( TYPE_TimerInfo );
int Timer_TimerFind( word chId, word evtId, dword timerStart, dword timerEnd, dword eventStart, dword eventEnd );

#endif
