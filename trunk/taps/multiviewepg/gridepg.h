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

#ifndef __GRIDEPG__
  #define __GRIDEPG__

#define __GRIDEPG_VERSION__ "2008-01-15"

static bool GridEPG_Active = FALSE;
static bool GridEPG_Redraw = FALSE;
static int GridEPG_SelectedCol = 0;
static int GridEPG_PrevSelectedCol = 0;
static int GridEPG_SelectedRow = 0;
static int GridEPG_PrevSelectedRow = 0;
static int GridEPG_Min = 0;
static int GridEPG_Max = 0;
static int GridEPG_TvChannels = 0;
static int GridEPG_RdChannels = 0;
static int GridEPG_Mode = SVC_TYPE_Tv;
static dword GridEPG_LastRendered = 0;
static int GridEPG_AppendTimerNum = 0;
static dword GridEPG_SelectColTime = 0;
static int GridEPG_Size = 180;
static int GridEPG_Type = 0;	// Used to store the mode i.e. TV Radio despite not coding for radio

#define GridEPG_AltNum 8
#define GridEPG_AltHeight 39

static TYPE_TimerInfo GridEPG_RestoreTimer;
static int GridEPG_Channels[1000];
static int GridEPG_Channels_ptr = 0;
static bool GridEPG_ChannelExcludesChanged = FALSE;
static bool GridEPG_ChannelsBuilt = FALSE;
static int GridEPG_ChannelExcludes[200];
static int GridEPG_ChannelExcludes_ptr = 0;
static dword GridEPG_StartTime = 0;

static int GRIDEPG_EVTWIDTH = 520;
static int GRIDEPG_CHANNELWIDTH = 60;

// Number of channels is adjustable therefore the height is also adjustable
static int GRIDEPG_CHANNELS = GridEPG_AltNum;//10;
static int GRIDEPG_ROWHEIGHT = GridEPG_AltHeight;//28;
static int GRIDEPG_EVTSTART = 0;

#define GRIDEPG_GAP 2

enum {
	STARTBEFORE,
	MIDDLE,
	FINISHAFTER,
	STARTENDOVER
};

static byte* GridEPG_ItemBase = NULL;

void GridEPG_MemFree( void );
void GridEPG_CreateGradients( void );
bool GridEPG_IsActive( void );
void GridEPG_Close( void );
dword GridEPG_Keyhandler( dword param1 );
int GridEPG_RenderItem( int rgn, dword starttime, int duration, int channel, char * event, bool selected );
void GridEPG_RenderChannel( int rgn, int channel );
void GridEPG_RenderEvents( int rgn, int channel );
void GridEPG_RenderRow( int rgn, int channel );
void GridEPG_Render( void );
void GridEPG_Activate( void );
void GridEPG_ReturnStart( void );
void GridEPG_RenderBlankRow( int rgn, int channel );
int GridEPG_NumEvents(  int channel );
int GridEPG_GetEventPointer( void );
void GridEPG_DeleteTimerCallback(int option);
void GridEPG_NewFavourite( void );
void GridEPG_FavNewReturn( favitem a, bool cancel );
void GridEPG_TimerEditReturn( int retval, bool cancel );
void GridEPG_AppendCallback( bool yes );
void GridEPG_CreateTimer( int row );
void GridEPG_TimerConflictCallback( bool result );
void GridEPG_InstantRecordCallback( bool yes );
void GridEPG_ChangeChannel( int row );
void GridEPG_RenderTimeLoc( void );
void GridEPG_SelectedColChange( void );
void GridEPG_SelectedRowChange( void );
void GridEPG_BuildExcludes( void );
char * GridEPG_ExportExcludes( void );
bool GridEPG_ExcludeChannel( int channel );
void GridEPG_TimerAppendConflictCallback(bool result);
void GridEPG_RemoveRecEventCallback(bool yes);
void GridEPG_RemoveChannel( bool yes );
void GridEPG_BuildChannels( void );
void GridEPG_RenderTimers(int rgn) ;
void GridEPG_RenderBottomPanel(int rgn, int item);
void GridEPG_ResetChannels( bool yes );
void GridEPG_RecordingEditReturn( void );
void GridEPG_SwitchMode(void);
void GridEPG_RenderEmptyBottomPanel(int);
void GridEPG_AddExclude( int svcType, int svcNum );
int GridEPG_GetTimer( int row );
void GridEPG_Idle( void );

#endif
