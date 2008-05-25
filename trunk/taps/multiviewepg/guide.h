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

#ifndef __GUIDESTUFF__
  #define __GUIDESTUFF__

#define __GUIDESTUFF_VERSION__ "2008-01-15"

#define GUIDE_ITEMS 10

enum {
	SEARCH_TITLE,
	SEARCH_DESC,
	SEARCH_NONE
};

TYPE_TapEvent *guideevents[MAX_EVENTS];	// Store an array of the events
static int guideevents_ptr = 0;
static bool Guide_HasEvents = FALSE;
static int Guide_Mode = 0;
static int Guide_Chan = 0;
int Guide_MinRow = 0;
int Guide_MaxRow = GUIDE_ITEMS;
int Guide_SelectedRow = 0;
int Guide_SelectedPrevRow = 0;
static bool Guide_Redraw = FALSE;
static bool Guide_Rendered = FALSE;
static bool Guide_ExtShowing = FALSE;
static int Guide_SearchActive = SEARCH_NONE;
static dword lastPress = 0;
static dword prevPress = 0;
static int Guide_AppendTimerNum = 0;
static bool Guide_ShowingTimers = FALSE;
static TYPE_TimerInfo Guide_RestoreTimer;
static dword Guide_LastRendered = 0;

static byte* Guide_BaseLin = NULL;
static byte* Guide_HighLin = NULL;
static byte* Guide_BaseRight = NULL;

static byte* Guide_PartialBase = NULL;
static byte* Guide_RecBase = NULL;
static byte* Guide_PartialHigh = NULL;
static byte* Guide_RecHigh = NULL;
static byte* Guide_RecNotAval = NULL;
static byte* Guide_PartialNotAval = NULL;

bool Guide_isActive( void );
void Guide_ResetLoc( void );
void Guide_RenderTitle( void );
void Guide_CloseFullDetails( void );
void Guide_RenderFullDetails( int );
void Guide_RenderEventDetails( int );
void Guide_RenderItem( word, int, int, int, bool );
void Guide_RenderDisplay( void );
void Guide_ChangeChannel( int );
void Guide_MoveToPrevChar( char *, int );
void Guide_MoveToNextChar( char *, int );
void Guide_MoveToPrevChan( int );
void Guide_MoveToNextChan( int );
void Guide_KeyboardClose( char *, bool );
int Guide_KeyboardSearch( int );
void Guide_SearchString( void );
dword Guide_KeyHandler( dword );
void Guide_SearchStringIdle( void );
void Guide_RenderHelp( void );
bool Guide_GetTimer( int );
void Guide_CreateTimer( int );
void Guide_TimerConflictCallback( bool );
void Guide_AppendCallback( bool );
void Guide_RenderNextTimers( word );
void Guide_RenderNextTimersInBottom( word );
void Guide_RenderNextTimersInBottomCount( word rgn, int num );
void Guide_Activate( void );
int Guide_KeyboardSearchAnywhere( int max );
void Guide_TimerAppendConflictCallback(bool result) ;
void Guide_RemoveRecEventCallback(bool yes);
void Guide_MemFree( void );
void Guide_CreateGradients( void );
void Guide_RenderBlankItem( word rgn, int position );
void Guide_RenderBlankHighItem( word rgn, int position );
void Guide_RecordingEditReturn( void );
void Guide_MoveToCurrent( void );
void Guide_RecordingNowDisplay( void );
TYPE_TapEvent Guide_GetEvent( int item );
void Guide_PopulateList( void );
void Guide_SortByChannel( void );
void Guide_SortByName( void );
int Guide_GetEventSvcType( int item );
int Guide_GetEventSvcNum( int item );
int Guide_GetEventSvcId( int item );
char * Guide_GetEventName( int item );
char * Guide_GetEventDesc( int item );
dword Guide_GetEventStartTime( int item );
dword Guide_GetEventEndTime( int item );
void Guide_SortByRating( void );
void Guide_SortByStart( void );
int Guide_GetEventPointer( void );
int Guide_GetEventPointerRow( int row );
void Guide_RenderBlankEventDetails( void );
void Guide_Idle( void );
void Guide_DeleteTimerCallback( int option );

#endif
