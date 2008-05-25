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

#ifndef __EVENTSSTUFF__
  #define __EVENTSSTUFF__

#define __EVENTSSTUFF_VERSION__ "2008-01-15"

#define MAX_EVENTS 14000

enum{
	DATA_NONE,
	DATA_COLLECTED
};

enum{
	SORT_NAME,
	SORT_CHANNEL,
	SORT_START,
	SORT_RATING,
	SORT_TYPES
};

// Variables for the event section of the TAP
TYPE_TapEvent *_evtInfo;
TYPE_TapEvent *events[MAX_EVENTS];	// Store an array of the events
int eventsPointer = 0;
int _svcType, _svcNum, _eventNum;
char _strBuf[32];
int dataCollected = DATA_NONE;
int sortOrder = SORT_CHANNEL;
bool sorted = FALSE;
static bool Events_Loading = FALSE;
static bool Events_Reloaded= FALSE;
static dword Events_ReloadLastCheck = 0;
static word mjd;
static byte hour;
static byte min;
static byte sec;

// Function defines
void Events_FreeMemory( void );
void Events_Get( void );
TYPE_TapEvent Events_GetEvent( int );
void Events_GetBackground( void );
dword Events_GetEventStartTime( int );
int Events_GetEventSvcId( int );
int Events_GetEventSvcType( int );
int Events_GetEventSvcNum( int );
void Events_InsertEvent( TYPE_TapEvent, int, int );
int Events_MaxPerChannel( void );
void Events_ReloadHourIdle( void );
void Events_RemoveOld( void );
char* Events_SortOrderText( int );
void Events_TAPExit( void );
void Events_SortByChannel( void );
void Events_SortByChannelBackground( void );

#endif
