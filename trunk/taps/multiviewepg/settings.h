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

#ifndef __SETTINGS__
  #define __SETTINGS__

#define __SETTINGS_VERSION__ "2008-01-15"

#define OPTIONS_FILENAME "multiviewepg.ini"

#include "events.h"

// User configurable via settings. File to be saved and read for these
static bool Setting_ShowPartialTimers = TRUE;
static bool Setting_PIG = TRUE;
static int Setting_DefaultOrder = SORT_CHANNEL;
static int Setting_MessageTimeout = 200;
static int Setting_SearchDelay = 100;
static bool Setting_FadeIn = TRUE;
static bool Setting_FadeOut = TRUE;
static bool Settings_LoadFile = TRUE;
static dword Settings_LinearEPGKey = RKEY_Fav;
static dword Settings_TimerListKey = RKEY_TvRadio;
static dword Settings_RSSListKey = RKEY_Teletext;
static dword Settings_FavListKey = RKEY_Fav;
static dword Settings_GridEPGKey = RKEY_Subt;
static dword Settings_MultiListEPGKey = 0;
static int Settings_GridEPGSize = 180;
static int Settings_GradientFactor = 2;
static int Settings_NormalFonts = TRUE;
static bool Settings_ListAllTimers = FALSE;
static int Settings_StartBuffer = 3;
static int Settings_EndBuffer = 10;
static int Settings_OffsetX = 0;
static int Settings_OffsetY = 0;
static bool Settings_AppendEpisode = FALSE;
static bool Settings_RenameTimers = TRUE;
static bool Settings_RenderBackground = FALSE;
static int Settings_GridEPGChannels = 10;
static int Settings_LoadDelay = 100;
static int Settings_ReloadHour = 3;
static bool Settings_GridEPGLogos = TRUE;
static bool Settings_TimerListLogos = TRUE;

#define OSD_RedrawDelay 6000

static bool Settings_LinearEPGTimes = TRUE;
static char Settings_ChannelExcludes[256];



void Settings_Save( void );
void Settings_Defaults( void );
void Settings_Load( void );

#endif
